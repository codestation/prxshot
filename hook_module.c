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

#include <pspsysmem.h>
#include <pspthreadman.h>
#include <pspmodulemgr.h>
#include <psputilsforkernel.h>
#include <string.h>
#include "hook_module.h"
#include "payload.h"
#include "logger.h"

// payload memory id
SceUID payload_id = -1;
// payload memory address
void *payload_addr = NULL;
// previous module handler
STMOD_HANDLER previous = NULL;
// eboot found flag
int module_found = 0;
// opcode
u32 opcode_a = NOP;
u32 opcode_b = NOP;

// module_start addr
void *start_addr = NULL;

void restore_module_start() {
    _sw(opcode_a, (u32)start_addr);
    _sw(opcode_b, (u32)start_addr + 4);
    sceKernelDcacheWritebackInvalidateRange(start_addr, 8);
    sceKernelIcacheInvalidateRange(start_addr, 8);
}

int module_start_handler(SceModule2 *module) {
    // find first module loaded into user memory address
    // excluding the sceKernelLibrary module
    if(!module_found && strcmp(module->modname, "sceKernelLibrary") &&
            (module->text_addr & 0x80000000) != 0x80000000) {
        module_found = 1;
        // get the entry address
        start_addr = module->module_start;
        // get the original opcodes before patching them
        opcode_a = _lw((u32)start_addr);
        opcode_b = _lw((u32)start_addr+4);
        // patch the module_start so it jumps to our payload
        MAKE_JUMP((u32)start_addr, payload_addr);
        // create a empty delay slot
        _sw(NOP, (u32)start_addr + 4);
        // flush the cache
        sceKernelDcacheWritebackInvalidateRange(start_addr, 8);
        sceKernelIcacheInvalidateRange(start_addr, 8);
        // calculate the return jump offset
        int return_offset = &asm_hook_return_addr - &asm_hook_func;
        // patch the end of the payload so it can jump and continue
        // the module_start code
        MAKE_JUMP((u32)payload_addr + return_offset, start_addr);
    }
    return previous ? previous(module) : 0;
}

void *create_payload(void *payload_start, void *payload_end) {
    // calculate the size of the payload code
    int payload_size = payload_end - payload_start;
    // allocate the memory to hold the payload
    int part = sceKernelGetModel() > 0 ? PSP_MEMORY_PARTITION_UMDCACHE : PSP_MEMORY_PARTITION_USER;
    payload_id = sceKernelAllocPartitionMemory(part, "user_wrap", PSP_SMEM_High, payload_size, NULL);
    if(payload_id < 0)
        return NULL;
    void *block_addr = sceKernelGetBlockHeadAddr(payload_id);
    // copy the payload to the newly allocated block
    memcpy(block_addr, payload_start, payload_size);
    return block_addr;
}

int delete_payload_hook() {
    return payload_id >= 0 ? sceKernelFreePartitionMemory(payload_id) : -1;
}

void hook_module_start(void *syscall_addr) {
    payload_addr = create_payload(asm_hook_func, asm_hook_end);
    if(payload_addr) {
        // calculate the syscall offset
        int syscall_offset = &asm_hook_syscall_addr - &asm_hook_func;
        int syscall_number = sceKernelQuerySystemCall(syscall_addr);
        // patch the payload code with a syscall to our code
        MAKE_SYSCALL((u32)payload_addr + syscall_offset, syscall_number);
        // register the start handler to intercept the eboot
        previous = sctrlHENSetStartModuleHandler(module_start_handler);
    }
}
