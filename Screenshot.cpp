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

#include <pspdisplay.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "Screenshot.hpp"
#include "GlobalBuffer.hpp"
#include "SceIo.hpp"
#include "bitmap.h"
#include "logger.h"

#define BMP_SIZE 391734

void Screenshot::setPath(const char *s_path, const char *s_format) {
    free(path);
    path = strdup(s_path);
    free(format);
    format = strdup(s_format);
    reset();
}

void Screenshot::reset() {
    last_id = 0;
    updateFilename();
}

int Screenshot::updateFilename() {
    int end = last_id;
    do {
        sprintf(filename, format, path, last_id++);
        if(last_id == max_id)
            last_id = 0;
        SceIo fd;
        if(!fd.open(filename, SceIo::FILE_READ)) {
            kprintf("Using %s as name\n", filename);
            return last_id;
        }
        fd.close();
    } while(end != last_id);
    return -1;
}

bool Screenshot::takePicture(PspHandler::boot_type type) {
    GlobalBuffer buffer;
    void *mem = buffer.alloc(BMP_SIZE, type == PspHandler::DISC ? GlobalBuffer::MODE_GAME : GlobalBuffer::MODE_XMB);
    void *frame_addr;
    int frame_width, pixel_format;
    if(mem) {
        sceDisplayWaitVblankStart();
        if(sceDisplayGetFrameBuf(&frame_addr, &frame_width, &pixel_format, PSP_DISPLAY_SETBUF_NEXTFRAME) >= 0 && frame_addr) {
            unsigned int ptr = (unsigned int)frame_addr;
            ptr |= ptr & 0x80000000 ?  0xA0000000 : 0x40000000;
            bitmapWrite((void *)ptr, mem, pixel_format, filename);
            return true;
        }
    }
    return false;
}

Screenshot::~Screenshot() {
    free(path);
    free(format);
}
