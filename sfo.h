/*
 * sfo.h
 *
 *  Created on: 13/02/2011
 *      Author: code
 */

#ifndef SFO_H_
#define SFO_H_

#include <pspiofilemgr.h>

#define SFO_SIZE 236

struct sfo_index {
    unsigned short key_offset;
    unsigned char alignment;
    unsigned char data_type;
    unsigned int value_size;
    unsigned int value_size_with_padding;
    unsigned int data_offset;
}__attribute__((packed));

struct sfo {
    char id[4];
    unsigned int version;
    unsigned int key_offset;
    unsigned int value_offset;
    unsigned int pair_count;
}__attribute__((packed));

int read_sfo(SceUID fd, char *buffer, int size);

#endif /* SFO_H_ */
