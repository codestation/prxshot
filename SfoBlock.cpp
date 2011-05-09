/*
 * SfoBlock.cpp
 *
 *  Created on: 28/04/2011
 *      Author: code
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
    kprintf("SfoBlock Loading %s\n", file);
    if(!memcmp(file, "disc0", 5)) {
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

bool SfoBlock::prepare(int sfo_size, int keys_size) {
    size = sfo_size;
    data_block = (char *)malloc(sfo_size);
    key_offset = sizeof(sfo_header);
    value_offset = key_offset + ALIGN(keys_size, 4);
    return (data_block);
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

bool SfoBlock::getStringValue(const char *key, char *value, int size) {
    const char *key_offset = (data_block + header->key_offset);
    for(unsigned int i = 0; i < header->pair_count; i++) {
        if(!strcmp(key_offset + index[i].key_offset, key)) {
            strncpy(value, (const char *)(data_block + header->value_offset + index[i].data_offset), size);
            value[size-1] = 0;
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

void SfoBlock::setIntValue(const char *key, int value) {
    strcpy(data_block + key_offset, key);
    key_offset += strlen(key) + 1;
    *(int *)(data_block + value_offset) = value;
    value_offset += 4;
}

void SfoBlock::setStringValue(const char *key, const char *value, str_type type) {
    strcpy(data_block + key_offset, key);
    key_offset += strlen(key) + 1;
    strcpy(data_block + value_offset, value);
    switch(type) {
    case STR_TITLE:
        value_offset += TITLE_SIZE;
        break;
    case STR_NORMAL:
    default:
        value_offset += ALIGN(strlen(value), 4);
    }
}

SfoBlock::~SfoBlock() {
    delete[] data_block;
}
