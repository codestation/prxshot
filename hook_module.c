/*
 *  prxshot module
 *
 *  Copyright (C) 2011  Codestation
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pspsdk.h>
#include <pspsysmem.h>
#include <pspthreadman.h>
#include <pspmodulemgr.h>
#include <psputilsforkernel.h>
#include <pspinit.h>

#include <string.h>

#include "pspdefs.h"
#include "hook_module.h"
#include "payload.h"
#include "logger.h"

// previous module handler
STMOD_HANDLER previous = NULL;
// eboot found flag
int module_found = 0;
// original opcodes from module_start
u32 opcode_a = NOP;
u32 opcode_b = NOP;
u32 opcode_c = NOP;
// module_start addr
void *start_addr = NULL;
// syscall number
int syscall_number = -1;

void prxshot_restore_patch() {
    int k1 = pspSdkSetK1(0);
    // restore the patched opcodes in the module_start func
    kprintf("Restoring opcodes of module_start\n");
    _sw(opcode_a, (u32)start_addr);
    _sw(opcode_b, (u32)start_addr + 4);
    _sw(opcode_b, (u32)start_addr + 8);
    // flush the cache
    sceKernelDcacheWritebackInvalidateRange(start_addr, 12);
    sceKernelIcacheInvalidateRange(start_addr, 12);
    pspSdkSetK1(k1);
}

void create_stack_payload(void *user_stack) {
    int payload_size = prxshot_save_argp - prxshot_stack_func;
    void *payload_addr = user_stack - payload_size;
    // make a jump from module_start+8 to the payload code into the stack
    MAKE_JUMP((u32)start_addr+8, payload_addr);
    // flush the cache
    sceKernelDcacheWritebackInvalidateRange(start_addr+8, 4);
    sceKernelIcacheInvalidateRange(start_addr+8, 4);
    // copy the payload code into the user stack
    kprintf("Creating payload in user stack\n");
    memcpy(payload_addr, prxshot_stack_func, payload_size);
    // create a syscall to restore the module_start
    MAKE_SYSCALL((u32)payload_addr, sceKernelQuerySystemCall(prxshot_restore_patch));
    // create a jump into the payload code to return to the module_start
    int return_offset = prxshot_return_addr - prxshot_stack_func;
    MAKE_JUMP((u32)payload_addr + return_offset, start_addr);
    // flush the cache
    sceKernelDcacheWritebackInvalidateRange(payload_addr, payload_size);
    sceKernelIcacheInvalidateRange(payload_addr, payload_size);
}

void syscall_save_argp(int args, const char *argp, void *user_stack);

int module_start_handler(SceModule2 *module) {
    // find first module loaded into user memory address
    // excluding the sceKernelLibrary module
    kprintf("Load module: %s\n", module->modname)
    if(!module_found && sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_POPS) {
        module_found = 1;
        const char *path = sceKernelInitFileName();
        int len = 0;
        if(path) {
            len = strlen(path);
            kprintf("POPS found: %s\n", path);
        }
        syscall_save_argp(len, path, NULL);
    }
    if(!module_found &&
            (module->text_addr & 0x80000000) != 0x80000000 &&
            strcmp(module->modname, "sceKernelLibrary") &&
            //FIXME: find a better way to filter out these loaders
            // blacklist the aLoader plugin
            strcmp(module->modname, "aLoader") &&
            // blacklist open idea loader
            strcmp(module->modname, "OpenIdeaController") &&
            strcmp(module->modname, "ISO Loader Eboot") &&
            // blacklist the Prometheus iso loader
            strcmp(module->modname, "PLoaderGUI")) {
        module_found = 1;
        // get the entry address
        start_addr = module->module_start;
        // very old eboots that doesn't have module_start entry point
        if((u32)start_addr == 0xFFFFFFFF)
            start_addr = (void *)module->entry_addr;
        kprintf("Game found: %s, addr: %08X\n", module->modname, (u32)start_addr);
        // get the original opcodes before patching them
        opcode_a = _lw((u32)start_addr);
        opcode_b = _lw((u32)start_addr+4);
        opcode_c = _lw((u32)start_addr+8);
        // make an opcode to store the $sp register into $a2
        kprintf("Patching module_start\n");
        _sw(MV_A2SP_OPCODE, (u32)start_addr);
        // patch the module_start with a syscall to our code
        MAKE_SYSCALL((u32)start_addr+4, sceKernelQuerySystemCall(prxshot_save_argp));
        // flush the cache
        sceKernelDcacheWritebackInvalidateRange(start_addr, 8);
        sceKernelIcacheInvalidateRange(start_addr, 8);
    }
    return previous ? previous(module) : 0;
}

void hook_module_start() {
    kprintf("Hook created\n");
    previous = sctrlHENSetStartModuleHandler(module_start_handler);
}
