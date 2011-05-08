/*
 * Thread.h
 *
 *  Created on: 07/05/2011
 *      Author: code
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pspkerneltypes.h>
#include <pspthreadman.h>

class Thread {
    SceUID thread_id;
    static int _run(SceSize args, void *argp);
protected:
    virtual int run() = 0;
public:
    Thread() {}
    void start(const char *name, int priority = 0x10, int stack_size = 0x1000);
    static void delay(unsigned int usecs) { sceKernelDelayThread(usecs); }
    void stop();
};

#endif /* THREAD_H_ */
