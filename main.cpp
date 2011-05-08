#include <pspsdk.h>
#include "ScreenshotThread.hpp"
#include "kmalloc.h"
#include "logger.h"

PSP_MODULE_INFO("prxshot", 0x1000, 0, 4);
KMALLOC_HEAP_SIZE_KB(8);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(8);

ScreenshotThread *th;

extern "C" int module_start(SceSize args, void *argp) {
    kprintf("PRXshot start\n");
    libc_init();
    th = new ScreenshotThread(args, argp);
    kprintf("Starting Screenshot thread\n");
    th->start("prxshot", 0x10, 0x1000);
    return 0;
}

extern "C" int module_stop(SceSize args, void *argp) {
    th->stop();
    delete th;
    libc_finish();
    return 0;
}
