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

#ifndef SFOBLOCK_H_
#define SFOBLOCK_H_

#include "SceIo.hpp"

//#define VERSION_SIZE 4
//#define PARENTAL_LEVEL_SIZE 4
//#define PSCM_VALUES_TOTAL_SIZE (PARENTAL_LEVEL_SIZE + TITLE_SIZE + VERSION_SIZE)

class SfoBlock {
public:
    SfoBlock() : data_block(NULL) {};
    ~SfoBlock();
    const char *getData() { return data_block; }
    void load(SceIo *fd, int size);
    SceSize load(const char *file);
    bool save(SceIo *fd);
    bool prepare(int sfo_size, int pair_count, int keys_size);
    bool getIntValue(const char *key, int *value);
    const char *getStringValue(const char *key);
    void setIntValue(const char *key, int value);
    void setStringValue(const char *key, const char *value);
private:
    struct sfo_index {
        unsigned short key_offset;
        unsigned char alignment;
        unsigned char data_type;
        unsigned int value_size;
        unsigned int value_size_with_padding;
        unsigned int data_offset;
    }__attribute__((packed));

    struct sfo_header {
        char id[4];
        unsigned int version;
        unsigned int key_offset;
        unsigned int value_offset;
        unsigned int pair_count;
    }__attribute__((packed));

    sfo_header *header;
    sfo_index *index;
    char *data_block;
    int index_count;
    unsigned int key_offset;
    unsigned int value_offset;
    unsigned int size;
};

#endif /* SFOBLOCK_H_ */
