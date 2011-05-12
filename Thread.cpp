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
