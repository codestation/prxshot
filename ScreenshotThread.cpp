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

#include <string.h>
#include "ScreenshotThread.hpp"
#include "Settings.hpp"
#include "PspHandler.hpp"
#include "PbpBlock.hpp"
#include "SceIo.hpp"
#include "logger.h"

ScreenshotThread::ScreenshotThread(int args, void *argp) {
    this->argp = new char[args];
    strcpy(this->argp, (char *)argp);
    shot_path = NULL;
}

char *ScreenshotThread::createScreenshotDir(const char *gameid) {
    char *path = new char[32];
    if(settings->forceMemoryStick() || psp.getModel() < PspHandler::MODEL_GO) {
        strcpy(path, "ms0:");
    } else {
        strcpy(path, "ef0:");
    }
    strcat(path,"/PSP/SCREENSHOT");
    SceIo::mkdir(path);
    strcat(path, "/");
    strcat(path, gameid);
    return path;
}

void ScreenshotThread::prepareDirectory() {
    delete pbp;
    pbp = (psp.bootFrom() == PspHandler::DISC) ? new PbpBlock() : new PbpBlock(psp.getPBPPath());
    pbp->load();
    delete[] shot_path;
    shot_path = createScreenshotDir(pbp->getSFO()->getStringValue("DISC_ID"));
    SceIo::mkdir(shot_path);
    pbp->outputDir(shot_path);
    kprintf("Shot_path: %s, format: %s\n", shot_path, settings->getScreenshotFormat());
    screen->setPath(shot_path, settings->getScreenshotFormat());
}

int ScreenshotThread::run() {
    settings = new Settings((const char *)argp);
    settings->loadDefaults();
    if(settings->forceMemoryStick())
        SceIo::mkdir("ms0:/PSP");
    screen = new Screenshot();
    prepareDirectory();
    kprintf("Starting loop\n");
    while(screen->getID()) {
        int keymask = psp.getKeyPress();
        if(psp.updated()) {
            prepareDirectory();
            settings->loadCustomKey(pbp->getSFO()->getStringValue("DISC_ID"));
        }
        if(PspHandler::isPressed(keymask, settings->getKeyPress())) {
            if(psp.applicationType() == PspHandler::VSH && SceIo::mkdir(shot_path) < 0)
                pbp->reset();
            kprintf("Taking screenshot\n");
            screen->takePicture();
            screen->updateFilename();
            if(!pbp->created()) {
                pbp->start("pscm_th");
            }
        }
        Thread::delay(100000);
    }
    return 0;
}

ScreenshotThread::~ScreenshotThread() {
    delete argp;
}
