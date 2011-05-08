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

#include <pspiofilemgr.h>
#include "logger.h"

#ifdef KPRINTF_ENABLED

char buffer_log[256];

int kwrite(const char *path, void *buffer, int buflen) {
    int written = 0;
    SceUID file;
    file = sceIoOpen(path, PSP_O_APPEND | PSP_O_CREAT | PSP_O_WRONLY, 0777);
    if(file >= 0) {
        written = sceIoWrite(file, buffer, buflen);
        sceIoClose(file);
    }
    return written;
}

void boot_info() {
    int boot = sceKernelBootFrom();
    int key = sceKernelInitKeyConfig();
    switch(boot) {
        case PSP_BOOT_MS:
            kprintf("Booting from Memory Stick\n");
            break;
        case PSP_BOOT_DISC:
            kprintf("Booting from UMD\n");
            break;
        case PSP_BOOT_FLASH:
            kprintf("Booting from Flash\n");
            break;
        default:
            kprintf("Booting from: %i\n", boot);
    }
    switch(key) {
        case PSP_INIT_KEYCONFIG_GAME:
            kprintf("Init mode: Game\n");
            break;
        case PSP_INIT_KEYCONFIG_VSH:
            kprintf("Init mode: VSH\n");
            break;
        case PSP_INIT_KEYCONFIG_POPS:
            kprintf("Init mode: POPS\n");
            break;
        default:
            kprintf("Init mode: %i\n", key);
    }
}

#endif
