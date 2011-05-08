#include <pspsdk.h>
#include "ScreenshotThread.hpp"
#include "kmalloc.h"

PSP_MODULE_INFO("prxshot", 0x1000, 0, 4);
KMALLOC_HEAP_SIZE_KB(8);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(0);

ScreenshotThread *th;

extern "C" int module_start(SceSize args, void *argp) {
    libc_init();
    th = new ScreenshotThread(args, argp);
    th->start("prxshot");
    return 0;
}

extern "C" int module_stop(SceSize args, void *argp) {
    th->stop();
    delete th;
    libc_finish();
    return 0;
}
