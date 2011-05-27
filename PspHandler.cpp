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
#include "PspHandler.hpp"

volatile int PspHandler::loader_found = 0;
STMOD_HANDLER PspHandler::previous = NULL;
PspHandler::state_type PspHandler::state = PspHandler::STATE_NONE;

PspHandler::PspHandler() {
    if(applicationType() != VSH) {
        pbp_path = strdup(sceKernelInitFileName());
    }
    if(applicationType() != VSH && applicationType() != POPS && bootFrom() != DISC) {
        previous = sctrlHENSetStartModuleHandler(&module_start_handler);
    }
}

int PspHandler::module_start_handler(SceModule2 *module) {
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

int PspHandler::getKeyPress() {
    SceCtrlData pad;
    sceCtrlPeekBufferPositive(&pad, 1);
    return pad.Buttons;
}

bool PspHandler::isPressed(int buttons) {
    return isPressed(getKeyPress(), buttons);
}

void PspHandler::clearCache() {
    if(applicationType() == VSH) {
        sceIoDevctl("fatms0:", 0x0240D81E, NULL, 0, NULL, 0);
    }
}

PspHandler::~PspHandler() {
    free(pbp_path);
}
