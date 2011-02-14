/*
 *  MHP3patch user module
 *
 *  Copyright (C) 2010  Codestation
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

#include <pspiofilemgr.h>
#include <stdio.h>
#include "logger.h"

char buffer_log[256];

int logger(const char * string) {
    return appendLog(LOGFILE, (void*)string, strlen(string));
}

int appendLog(const char * path, void * buffer, int buflen) {
    int written = 0;
    SceUID file;
    file = sceIoOpen(path, PSP_O_APPEND | PSP_O_CREAT | PSP_O_WRONLY, 0777);
    if(file >= 0) {
        written = sceIoWrite(file, buffer, buflen);
        sceIoClose(file);
    }
    return written;
}
