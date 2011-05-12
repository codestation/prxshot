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
#include <string.h>
#include "PspHandler.hpp"

volatile int PspHandler::loader_found = 0;
STMOD_HANDLER PspHandler::previous = NULL;
PspHandler::state_type PspHandler::state = PspHandler::STATE_NONE;

PspHandler::PspHandler() {
    const char *filename = sceKernelInitFileName();
    pbp_path = new char[strlen(filename)+1];
    strcpy(pbp_path, filename);
    if(applicationType() != VSH && applicationType() != POPS && bootFrom() != DISC) {
        previous = sctrlHENSetStartModuleHandler(&module_start_handler);
    }
}

int PspHandler::module_start_handler(SceModule2 *module) {
    if((module->text_addr == 0x08804000 ||
        module->text_addr == 0x08900000) &&
        module->entry_addr != 0xFFFFFFFF) {
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

bool PspHandler::isPressed(unsigned int buttons) {
    return (getKeyPress() & buttons) == buttons;
}

PspHandler::~PspHandler() {
    delete[] pbp_path;
}
