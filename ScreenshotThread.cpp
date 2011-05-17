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

#include <psputils.h>
#include <stdio.h>
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
    strrchr(this->argp, '/')[1] = 0;
    shot_path = NULL;
    pbp = NULL;
    migrated = false;
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

char *sha1Key(const char *title) {
    char digest[20];
    char *buffer = new char[20];
    sceKernelUtilsSha1Digest((u8 *)title, strlen(title), (u8 *)digest);
    sprintf(buffer, "PS%08X", *(u8 *)digest);
    return buffer;
}

void ScreenshotThread::prepareDirectory() {
    delete pbp;
    if(psp.bootFrom() == PspHandler::DISC || psp.applicationType() == PspHandler::VSH) {
        kprintf("Loading PbpBlock\n");
        pbp = new PbpBlock();
    } else {
        kprintf("Loading PbpBlock, with path: %s\n", psp.getPBPPath());
        pbp = new PbpBlock(psp.getPBPPath());
    }
    if(psp.applicationType() == PspHandler::VSH)
        pbp->setSfoPath(argp);
    kprintf("Loading PBP/SFO\n");
    pbp->load();
    delete[] shot_path;
    kprintf("calling createScreenshotDir\n");
    if(psp.applicationType() == PspHandler::VSH) {
        shot_path = createScreenshotDir("XMB");
    } else {
        const char *gamekey = pbp->getSFO()->getStringValue("DISC_ID");
        // eboot found (Homebrew or PSN)
        if(psp.bootFrom() == PspHandler::DISC) {
            shot_path = createScreenshotDir(gamekey);
        }else {
            char *gen_id = sha1Key(pbp->getSFO()->getStringValue("TITLE"));
            if(!strcmp(gamekey, "USUSROCO")) {
                shot_path = createScreenshotDir(gen_id);
            } else {
                shot_path = createScreenshotDir(pbp->getSFO()->getStringValue("DISC_ID"));
                if(!migrated) {
                    char *olddir = createScreenshotDir(gen_id);
                    SceIo::rename(olddir, shot_path);
                    delete[] olddir;
                    migrated = true;
                }
            }
            delete []gen_id;
        }
    }
    SceIo::mkdir(shot_path);
    pbp->outputDir(shot_path);
    kprintf("Shot_path: %s, format: %s\n", shot_path, settings->getScreenshotFormat());
    screen->setPath(shot_path, settings->getScreenshotFormat());
}

int ScreenshotThread::run() {
    kprintf("ScreenshotThread started\n");
    settings = new Settings(argp);
    settings->loadDefaults();
    kprintf("Settings loaded\n");
    if(settings->forceMemoryStick())
        SceIo::mkdir("ms0:/PSP");
    screen = new Screenshot();
    prepareDirectory();
    kprintf("Starting loop\n");
    while(screen->getID()) {
        int keymask = psp.getKeyPress();
        if(psp.applicationType() != PspHandler::VSH && psp.updated()) {
            prepareDirectory();
            settings->loadCustomKey(pbp->getSFO()->getStringValue("DISC_ID"));
        }
        if(PspHandler::isPressed(keymask, settings->getKeyPress())) {
            if(psp.applicationType() == PspHandler::VSH && SceIo::mkdir(shot_path) == 0) {
                kprintf("XMB directory deleted\n");
                pbp->reset();
                screen->reset();
            }
            kprintf("Taking screenshot\n");
            screen->takePicture(psp.bootFrom());
            screen->updateFilename();
            if(!pbp->created()) {
                kprintf("Starting pbp thread\n");
                if(settings->clearCache())
                    pbp->onStop(&psp.clearCache);
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
