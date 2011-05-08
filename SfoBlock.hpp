/*
 * SfoBlock.h
 *
 *  Created on: 28/04/2011
 *      Author: code
 */

#ifndef SFOBLOCK_H_
#define SFOBLOCK_H_

#include "SceIo.hpp"

#define ALIGN(x, y) (((x) + ((y)-1)) & ~((y)-1))
#define TITLE_SIZE 128
#define VERSION_SIZE 4
#define PARENTAL_LEVEL_SIZE 4
#define PSCM_VALUES_TOTAL_SIZE (PARENTAL_LEVEL_SIZE + TITLE_SIZE + VERSION_SIZE)

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

class SfoBlock {
    sfo_header *header;
    sfo_index *index;
    char **values;
    char *data_block;
    unsigned int key_offset;
    unsigned int value_offset;
    unsigned int size;
public:
    enum str_type { STR_NORMAL, STR_TITLE };
    SfoBlock() : data_block(NULL) {};
    const char *getData() { return data_block; }
    void load(SceIo *fd, int size);
    SceSize load(const char *file);
    bool save(SceIo *fd);
    bool prepare(int sfo_size, int keys_size);
    bool getIntValue(const char *key, int *value);
    bool getStringValue(const char *key, char *value, int size);
    const char *getStringValue(const char *key);
    void setIntValue(const char *key, int value);
    void setStringValue(const char *key, const char *value, str_type type);
    ~SfoBlock();
};

#endif /* SFOBLOCK_H_ */
