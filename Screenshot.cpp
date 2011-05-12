/*
 * Screenshot.cpp
 *
 *  Created on: 06/05/2011
 *      Author: code
 */

#include <pspdisplay.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "Screenshot.hpp"
#include "GlobalBuffer.h"
#include "SceIo.hpp"
#include "bitmap.h"
#include "logger.h"

#define BMP_SIZE 391734

void Screenshot::setPath(const char *s_path, const char *s_format) {
    delete[] path;
    path = new char[strlen(s_path)+1];
    strcpy(path, s_path);
    delete[] format;
    format = new char[strlen(s_format)+1];
    strcpy(format, s_format);
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

bool Screenshot::takePicture() {
    GlobalBuffer buffer;
    void *mem = buffer.alloc(BMP_SIZE, GlobalBuffer::MODE_GAME);
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
    delete []path;
    delete []format;
}
