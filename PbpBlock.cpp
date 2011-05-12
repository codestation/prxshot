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
#include "PbpBlock.hpp"
#include "logger.h"

#define PSCM_FILE "/PSCM.DAT"
#define PSCM_SIZE 236
#define PSCM_PAIRS 3
#define PSCM_KEYS_SIZE 32

PbpBlock::PbpBlock() {
    init(NULL);
}

PbpBlock::PbpBlock(const char *path) {
    init(path);
}

void PbpBlock::init(const char *path) {
    is_created = false;
    header = new pbp_header;
    sfo = new SfoBlock();
    if(path) {
        file = new char[strlen(path)+1];
        strcpy(file, path);
    } else {
        file = NULL;
    }
}

void PbpBlock::outputDir(const char *path) {
    outfile = new char[strlen(path) + 1 + strlen(PSCM_FILE)];
    strcpy(outfile, path);
    strcat(outfile, PSCM_FILE);
}

int PbpBlock::run() {
    kprintf("Started PbpBlock thread\n");
    SceSize size;
    SceIo fdo, fdi;
    kprintf("opening %s\n", outfile);
    if(!fdo.open(outfile, SceIo::FILE_RDWR))
        return 1;
    kprintf("opening %s\n", file);
    if(file && (!fdi.open(file, SceIo::FILE_READ)))
        return 1;
    SfoBlock *pscm = generatePSCM(sfo);
    fdo.write(header, sizeof(pbp_header));
    pscm->save(&fdo);
    if(!file) {
        fdi.open(ICON0_PATH, SceIo::FILE_READ);
        size = fdi.size();
    } else {
        fdi.seek(header->icon0_offset, SceIo::FILE_SET);
        size = header->icon1_offset - header->icon0_offset;
    }
    header->icon0_offset = header->sfo_offset + PSCM_SIZE;
    header->icon1_offset = header->icon0_offset + size;
    header->pic0_offset = header->icon0_offset + size;
    appendData(&fdo, &fdi, size);
    if(!file) {
        fdi.close();
        fdi.open(PIC1_PATH, SceIo::FILE_READ);
        size = fdi.size();
    } else {
        fdi.seek(header->pic1_offset, SceIo::FILE_SET);
        size = header->snd0_offset - header->pic1_offset;
    }
    header->pic1_offset = header->pic0_offset;
    header->snd0_offset = header->pic1_offset + size;
    header->psp_offset = header->pic1_offset + size;
    header->psar_offset = header->pic1_offset + size;
    appendData(&fdo, &fdi, size);
    if(!file)
        fdi.close();
    fdo.rewind();
    fdo.write(header, sizeof(pbp_header));
    fdo.close();
    fdi.close();
    delete pscm;
    delete header;
    is_created = true;
    return 0;
}

bool PbpBlock::load() {
    SceIo fd;
    kprintf("PbpBlock::load, %s\n", file);
    if(file && !fd.open(file, SceIo::FILE_READ))
        return false;
    if(file) {
        fd.read(header, sizeof(pbp_header));
        sfo->load(&fd, header->icon0_offset - header->sfo_offset);
        fd.close();
    } else {
        kprintf("Loading: %s\n", SFO_PATH);
        SceSize size = sfo->load(SFO_PATH);
        if(!size) {
            return false;
        }
        memcpy(header->id, "\0PBP", 4);
        header->version = 0x10000;
        header->sfo_offset = sizeof(pbp_header);
        header->icon0_offset = header->sfo_offset + size;
    }
    is_created = false;
    return true;
}

void PbpBlock::appendData(SceIo *out, SceIo *in, size_t size) {
    int read = BUFFER_SIZE;
    char *buffer = new char[BUFFER_SIZE];
    while(size) {
        read = in->read(buffer, read);
        if(read <= 0) break;
        out->write(buffer, read);
        size -= read;
        read = size < BUFFER_SIZE ? size : BUFFER_SIZE;
    }
    delete buffer;
}

SfoBlock *PbpBlock::generatePSCM(SfoBlock *sfo) {
    int value;
    kprintf("Generating PSCM\n");
    SfoBlock *pscm = new SfoBlock();
    pscm->prepare(PSCM_SIZE, PSCM_PAIRS, PSCM_KEYS_SIZE);
    if(!sfo->getIntValue(PARENTAL_LEVEL, &value))
        value = 1;
    pscm->setIntValue(PARENTAL_LEVEL, value);
    char *title = new char[128];
    if(!sfo->getStringValue(TITLE, title, TITLE_SIZE))
        strcpy(title, "Game / Homebrew");
    pscm->setStringValue(TITLE, title, SfoBlock::STR_TITLE);
    delete title;
    if(!sfo->getIntValue(VERSION, &value))
        value = 0x10000;
    pscm->setIntValue(VERSION, value);
    return pscm;
}

PbpBlock::~PbpBlock() {
    delete file;
    delete header;
    delete sfo;
}
