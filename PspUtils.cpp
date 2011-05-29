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

#include <pspctrl.h>
#include <pspiofilemgr.h>
#include <string.h>
#include <malloc.h>
#include <psputils.h>
#include <pspthreadman.h>
#include <stdio.h>
#include "PspUtils.hpp"

volatile int PspUtils::loader_found = 0;
STMOD_HANDLER PspUtils::previous = NULL;
PspUtils::state_type PspUtils::state = PspUtils::STATE_NONE;
unsigned int PspUtils::button_timeout = 0;

PspUtils::PspUtils() {
    if(applicationType() != VSH) {
        pbp_path = strdup(sceKernelInitFileName());
        if(applicationType() != POPS && bootFrom() != DISC) {
            previous = sctrlHENSetStartModuleHandler(&module_start_handler);
        }
    }
}

int PspUtils::module_start_handler(SceModule2 *module) {
    if((module->text_addr == 0x08804000 ||    // base address for game eboots
        module->text_addr == 0x08900000) &&   // base address for some homebrews
        module->entry_addr != 0xFFFFFFFF &&   // skip some user mode prx that loads @ 0x08804000
        strcmp(module->modname, "opnssmp")) { // this loads @ 0x08804000 too
        if(state == STATE_NONE) {
            state = STATE_GAME;
        } else {
            state = STATE_LOADER;
            loader_found++;
        }
    }
    return previous ? previous(module) : 0;
}

int PspUtils::getKeyPress() {
    SceCtrlData pad;
    sceCtrlPeekBufferPositive(&pad, 1);
    return pad.Buttons;
}

bool PspUtils::isPressed(int buttons) {
    return isPressed(getKeyPress(), buttons, 0);
}

void PspUtils::clearCache() {
    if(applicationType() == VSH) {
        sceIoDevctl("fatms0:", 0x0240D81E, NULL, 0, NULL, 0);
    }
}

bool PspUtils::isPressed(int buttons, int mask, unsigned int msecs) {
    if(msecs == 0) {
        return (buttons & mask) == mask;
    } else {
        if((buttons & mask) == mask) {
            if(button_timeout == 0) {
                button_timeout = getMilliseconds();
            } else {
                if(getMilliseconds() - button_timeout > msecs) {
                    button_timeout = 0;
                    return true;
                }
            }
        } else {
            button_timeout = 0;
        }
    }
    return false;
}

char *PspUtils::sha1Key(const char *title) {
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

unsigned int PspUtils::getMilliseconds() {
    SceKernelSysClock clock;
    sceKernelGetSystemTime(&clock);
    unsigned int time = clock.low / 1000;
    time += clock.hi * (0xFFFFFFFF / 1000);
    return time;
}

PspUtils::~PspUtils() {
    free(pbp_path);
}
