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

#ifndef THREAD_H_
#define THREAD_H_

#include <pspkerneltypes.h>
#include <pspthreadman.h>

class Thread {
public:
    Thread() {}
    // default thread priority: 16, default stack size: 4KiB
    void start(const char *name, int priority = 0x10, int stack_size = 0x1000);
    static void delay(unsigned int usecs) { sceKernelDelayThread(usecs); }
    void stop();
protected:
    virtual int run() = 0;
private:
    static int _run(SceSize args, void *argp);
    SceUID thread_id;
};

#endif /* THREAD_H_ */
