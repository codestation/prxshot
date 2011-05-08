/*
 * Thread.cpp
 *
 *  Created on: 07/05/2011
 *      Author: code
 */

#include <pspthreadman.h>
#include "Thread.hpp"
#include "logger.h"

void Thread::start(const char *name, int priority, int stack_size) {
    if((thread_id = sceKernelCreateThread(name, &_run, priority, stack_size, 0, 0)) >= 0) {
        Thread *thisptr = this;
        // this function makes a copy of the data passed on the 3rd arg
        // so we can't pass "this" directly.
        sceKernelStartThread(thread_id, 4, &thisptr);
    }
}

int Thread::_run(SceSize args, void *argp) {
    // manage the double indirection caused by sceKernelStartThread
    return static_cast<Thread **>(argp)[0]->run();
}

void Thread::stop() {
    sceKernelTerminateThread(thread_id);
}
