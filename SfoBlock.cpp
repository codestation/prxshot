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
#include <malloc.h>
#include "SfoBlock.hpp"
#include "Thread.hpp"
#include "logger.h"

void SfoBlock::load(SceIo *fd, int sfo_size) {
    size = sfo_size;
    data_block = new char[sfo_size];
    fd->read(data_block, sfo_size);
    header = (sfo_header *)data_block;
    index = (sfo_index *)(data_block + sizeof(sfo_header));
}

SceSize SfoBlock::load(const char *file) {
    SceIo fd;
    if(!memcmp(file, "disc0", 5)) {
        // wait until the UMD is ready
        while(!fd.open(file, SceIo::FILE_READ)) {
            Thread::delay(100000);
        }
    } else {
        if(!fd.open(file, SceIo::FILE_READ)) {
            return 0;
        }
    }
    SceSize size = fd.size();
    load(&fd, size);
    fd.close();
    return size;
}

bool SfoBlock::save(SceIo *fd) {
    return (fd->write(data_block, size) > 0);
}

bool SfoBlock::getIntValue(const char *key, int *value) {
    const char *key_offset = (data_block + header->key_offset);
    for(unsigned int i = 0; i < header->pair_count; i++) {
        if(!strcmp(key_offset + index[i].key_offset, key)) {
            *value = *(int *)(data_block + header->value_offset + index[i].data_offset);
            return true;
        }
    }
    return false;
}

const char *SfoBlock::getStringValue(const char *key) {
    const char *key_offset = (data_block + header->key_offset);
    for(unsigned int i = 0; i < header->pair_count; i++) {
        if(!strcmp(key_offset + index[i].key_offset, key)) {
            return (const char *)(data_block + header->value_offset + index[i].data_offset);
        }
    }
    return NULL;
}

bool SfoBlock::prepare(int sfo_size, int pair_count, int keys_size) {
    size = sfo_size;
    data_block = new char[sfo_size];
    header = (sfo_header *)data_block;
    index = (sfo_index *)(data_block + sizeof(sfo_header));
    memcpy(header->id, "\0PSF", 4);
    header->version = 0x0101;
    header->key_offset = sizeof(sfo_header) + (sizeof(sfo_index) * pair_count);
    header->value_offset = header->key_offset + keys_size;
    header->pair_count = pair_count;
    key_offset = 0;
    value_offset = 0;
    index_count = 0;
    return (data_block);
}

void SfoBlock::setIntValue(const char *key, int value) {
    kprintf("setIntValue, key: %s, value: %i\n", key, value);
    sfo_index *idx = &index[index_count];
    idx->key_offset = key_offset;
    idx->data_offset = value_offset;
    idx->data_type = 4;
    idx->alignment = 4;
    idx->value_size = 4;
    idx->value_size_with_padding = 4;

    strcpy((char *)(data_block + header->key_offset + key_offset), key);
    *(int *)(data_block + header->value_offset + value_offset) = value;
    key_offset += strlen(key) + 1;
    value_offset += 4;
    index_count++;
}

void SfoBlock::setStringValue(const char *key, const char *value) {
    kprintf("setStringsValue, key: %s, value: %s\n", key, value);
    sfo_index *idx = &index[index_count];
    idx->key_offset = key_offset;
    idx->data_offset = value_offset;
    idx->data_type = 2;
    idx->alignment = 4;
    idx->value_size = strlen(value) + 1;
    if(!strcmp(key, "TITLE"))
        idx->value_size_with_padding = TITLE_SIZE;
    else
        idx->value_size_with_padding = ALIGN(strlen(value), 4);
    strcpy((char *)(data_block + header->key_offset + key_offset), key);
    char *value_addr = (char *)(data_block + header->value_offset + value_offset);
    memset(value_addr, 0, idx->value_size_with_padding);
    strcpy(value_addr, value);
    index_count++;
    key_offset += strlen(key) + 1;
    value_offset += idx->value_size_with_padding;
}

SfoBlock::~SfoBlock() {
    delete[] data_block;
}
