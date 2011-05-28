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
#include "PspUtils.hpp"
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
    if(settings->forceMemoryStick() || psp.getModel() < PspUtils::MODEL_GO) {
        strcpy(path, "ms0:");
    } else {
        strcpy(path, "ef0:");
    }
    strcat(path,"/PSP/SCREENSHOT");
    // make sure that the /PSP/SCREENSHOT directory exists, as it doesn't
    // come by default (haven't tested if its created on reformat)
    SceIo::mkdir(path);
    strcat(path, "/");
    strcat(path, gameid);
    return path;
}

char *sha1Key(const char *title) {
    u32 digest[5];
    // FIXME: it must be strlen(title) instead of 128  (and the buffer could
    // be way smaller) but we have to maintain compatibility with older versions
    // of prxshot
    char *buffer = new char[128];
    memset(buffer, 0, 128);
    strcpy(buffer, title);
    sceKernelUtilsSha1Digest((u8 *)buffer, 128, (u8 *)digest);
    sprintf(buffer, "PS%08X", digest[0]);
    return buffer;
}

void ScreenshotThread::prepareDirectory() {
    delete pbp;
    if(psp.bootFrom() == PspUtils::DISC || psp.applicationType() == PspUtils::VSH) {
        kprintf("Loading PbpBlock\n");
        pbp = new PbpBlock(argp);
    } else {
        kprintf("Loading PbpBlock, with path: %s\n", psp.getPBPPath());
        pbp = new PbpBlock(argp, psp.getPBPPath());
    }
    kprintf("Loading PBP/SFO\n");
    pbp->load();
    delete[] shot_path;
    kprintf("calling createScreenshotDir\n");
    if(psp.applicationType() == PspUtils::VSH) {
        shot_path = createScreenshotDir("XMB");
    } else {
        const char *gamekey = pbp->getSFO()->getStringValue("DISC_ID");
        // eboot found (Homebrew or PSN)
        if(psp.bootFrom() == PspUtils::DISC) {
            shot_path = createScreenshotDir(gamekey);
        }else {
            char *gen_id = sha1Key(pbp->getSFO()->getStringValue("TITLE"));
            if(!strcmp(gamekey, "UCJS10041")) {
                shot_path = createScreenshotDir(gen_id);
            } else {
                shot_path = createScreenshotDir(pbp->getSFO()->getStringValue("DISC_ID"));
                // migrate the directories from the old PSXXXXXXXX format to GAME_ID
                // for PSN games
                if(!migrated) {
                    kprintf("Checking folder migration\n");
                    char *olddir = createScreenshotDir(gen_id);
                    kprintf("Moving %s to %s\n", olddir, shot_path);
                    if(SceIo::rename(olddir, shot_path) == 0) {
                        kprintf("Migration success\n");
                    }
                    delete[] olddir;
                    migrated = true;
                }
            }
            delete[] gen_id;
        }
    }
    //SceIo::mkdir(shot_path);
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
        if(psp.updated() && psp.applicationType() != PspUtils::VSH) {
            prepareDirectory();
            settings->loadCustomKey(pbp->getSFO()->getStringValue("DISC_ID"));
        }
        if(PspUtils::isPressed(keymask, settings->getKeyPress())) {
            if(psp.applicationType() == PspUtils::VSH && SceIo::mkdir(shot_path) == 0) {
                kprintf("XMB directory deleted\n");
                pbp->reset();
                screen->reset();
            } else {
                SceIo::mkdir(shot_path);
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
        //Thread::delay(100000);
        Thread::delay(16666);
    }
    return 0;
}

ScreenshotThread::~ScreenshotThread() {
    delete argp;
}
