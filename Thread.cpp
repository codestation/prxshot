/*
 * Thread.cpp
 *
 *  Created on: 07/05/2011
 *      Author: code
 */

#include <pspthreadman.h>
#include "Thread.hpp"

void Thread::start(const char *name, int priority, int stack_size) {
    if((thread_id = sceKernelCreateThread(name, &_run, priority, stack_size, 0, 0)) >= 0)
        sceKernelStartThread(thread_id, 0, this);
}

int Thread::_run(SceSize args, void *argp) {
    return static_cast<Thread *>(argp)->run();
}

void Thread::stop() {
    sceKernelTerminateThread(thread_id);
}
