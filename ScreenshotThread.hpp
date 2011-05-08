/*
 * ScreenshotThread.h
 *
 *  Created on: 07/05/2011
 *      Author: code
 */

#ifndef SCREENSHOTTHREAD_H_
#define SCREENSHOTTHREAD_H_

#include "Thread.hpp"
#include "PspHandler.hpp"
#include "Screenshot.hpp"

class ScreenshotThread: public Thread {
    char *argp;
    Screenshot *screen;
    PspHandler psp;
protected:
    int run();
public:
    ScreenshotThread(int args, void *argp);
    ~ScreenshotThread();
};

#endif /* SCREENSHOTTHREAD_H_ */
