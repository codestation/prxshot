/*
 * ScreenshotThread.cpp
 *
 *  Created on: 07/05/2011
 *      Author: code
 */
#include <pspctrl.h>
#include <string.h>
#include "ScreenshotThread.hpp"
#include "Settings.hpp"
#include "PspHandler.hpp"
#include "PbpBlock.hpp"
#include "SceIo.hpp"

ScreenshotThread::ScreenshotThread(int args, void *argp) {
    this->argp = new char[args+1];
    strcpy(this->argp, (char *)argp);
}

int ScreenshotThread::run() {
    Settings *settings = new Settings((const char *)argp);
    settings->loadDefaults();
    if(settings->forceMemoryStick())
        SceIo::mkdir("ms0:/PSP");
    screen = new Screenshot();
    PbpBlock *pbp = new PbpBlock();
    pbp->load();
    char id[16];
    pbp->getSFO()->setStringValue("GAME_ID", id, SfoBlock::STR_NORMAL);
    screen->setPath(id, settings->getScreenshotFormat());
    while(screen->getID()) {
        if(psp.isPressed(settings->getKeyPress())) {
            screen->takePicture();
            screen->updateFilename();
        }
    }
    return 0;
}

ScreenshotThread::~ScreenshotThread() {
    delete argp;
}
