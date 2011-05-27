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
#include "ScreenshotThread.hpp"
#include "kmalloc.h"
#include "logger.h"

PSP_MODULE_INFO("prxshot", PSP_MODULE_KERNEL, 0, 4);
PSP_MAIN_THREAD_STACK_SIZE_KB(4);
KMALLOC_HEAP_SIZE_KB(16);
PSP_HEAP_SIZE_KB(4);

ScreenshotThread *th;

extern "C" int module_start(SceSize args, void *argp) {
    kprintf("PRXshot start\n");
    int res = libc_init();
    if(res < 0) {
        kprintf("libc_init failed: %08X\n", res);
        return 1;
    }
    th = new ScreenshotThread(args, argp);
    kprintf("Starting Screenshot thread\n");
    th->start("prxshot");
    return 0;
}

extern "C" int module_stop(SceSize args, void *argp) {
    th->stop();
    delete th;
    libc_finish();
    return 0;
}
