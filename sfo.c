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

#include <pspkernel.h>
#include <string.h>
#include "sfo.h"
#include "pbp.h"
#include "logger.h"
/*
int read_sfo_id(SceUID fd, char *buffer, int size, char *id_buf, int id_size) {
    struct sfo *sfo_data = (struct sfo *)buffer;
    // read the sfo header
    sceIoRead(fd, sfo_data, sizeof(struct sfo));
    // allocate memory to read the sfo block
    void *sfo_block = buffer + size;
    sceIoRead(fd, sfo_block, size);
    // get the sfo index table inside the block
    struct sfo_index *index_block = sfo_block;
    unsigned int keys_offset_block = sizeof(struct sfo) + (sizeof(struct sfo_index) * sfo_data->pair_count);
    void *value_block = sfo_block + sfo_data->value_offset - sizeof(struct sfo);
    for(int i = 0; i < sfo_data->pair_count; i++) {
        char *key_addr = sfo_block + index_block[i].key_offset + keys_offset_block - sizeof(struct sfo);
        if(!strcmp(key_addr, "DISC_ID")) {
            memcpy(id_buf, value_block, id_size);
            return 1;
        }
        value_block += index_block[i].value_size_with_padding;
    }
    return 0;
}
*/
int read_sfo_title(SceUID fd, char *buffer, int size, char *id_buf, int id_size) {
    struct sfo *sfo_data = (struct sfo *)buffer;
    // read the sfo header
    sceIoRead(fd, sfo_data, sizeof(struct sfo));
    // allocate memory to read the sfo block
    void *sfo_block = buffer + size;
    sceIoRead(fd, sfo_block, size);
    // get the sfo index table inside the block
    struct sfo_index *index_block = sfo_block;
    unsigned int keys_offset_block = sizeof(struct sfo) + (sizeof(struct sfo_index) * sfo_data->pair_count);
    void *value_block = sfo_block + sfo_data->value_offset - sizeof(struct sfo);
    for(int i = 0; i < sfo_data->pair_count; i++) {
        char *key_addr = sfo_block + index_block[i].key_offset + keys_offset_block - sizeof(struct sfo);
        if(!strcmp(key_addr, "TITLE")) {
            memcpy(id_buf, value_block, id_size);
            return id_size;
        }
        value_block += index_block[i].value_size_with_padding;
    }
    return 0;
}

int read_sfo(SceUID fd, char *buffer, int size) {
    struct sfo *sfo_data = (struct sfo *)buffer;
    // read the sfo header
    sceIoRead(fd, sfo_data, sizeof(struct sfo));
    // allocate memory to read the sfo block
    void *sfo_block = buffer + size;
    sceIoRead(fd, sfo_block, size);
    // get the sfo index table inside the block
    struct sfo_index *index_block = sfo_block;
    unsigned int keys_offset_block = sizeof(struct sfo) + (sizeof(struct sfo_index) * sfo_data->pair_count);
    struct sfo_index *index_buffer = (struct sfo_index *)(buffer + sizeof(struct sfo));
    void *keys_buffer_start = buffer + sizeof(struct sfo) + (sizeof(struct sfo_index) * 3);
    int keys_count = 0;
    int keys_offset = 0;
    void *value_buffer = buffer + 100;
    void *value_block = sfo_block + sfo_data->value_offset - sizeof(struct sfo);
    for(int i = 0; i < sfo_data->pair_count; i++) {
        char *key_addr = sfo_block + index_block[i].key_offset + keys_offset_block - sizeof(struct sfo);
        if(!strcmp(key_addr, "PARENTAL_LEVEL") || !strcmp(key_addr, "TITLE")) {
            memcpy(&index_buffer[keys_count], &index_block[i], sizeof(struct sfo_index));
            index_buffer[keys_count].key_offset = keys_offset;
            index_buffer[keys_count].data_offset = (int)((char *)value_buffer - buffer) - 100;
            strcpy(keys_buffer_start + keys_offset, key_addr);
            keys_offset += strlen(key_addr) + 1;
            int value_size = 0;
            if(index_block[i].data_type == 4) {
                value_size = 4;
                memcpy(value_buffer, value_block, value_size);
            } else if(index_block[i].data_type == 2) {
                value_size = index_block[i].value_size_with_padding;
                // force the TITLE value to 128 bytes
                memset(value_buffer, 0, 128);
                if(index_buffer[keys_count].value_size == strlen(value_block)) {
                    // bugged sfo, while it works for the Game menu, it doesnt
                    // for the pictures directory
                    index_buffer[keys_count].value_size++;
                    value_size++;
                }
                memcpy(value_buffer, value_block, value_size);
                value_size = 128;
                index_buffer[keys_count].value_size_with_padding = 128;
            } else {
                break;
            }
            keys_count++;
            value_buffer += value_size;
        }
        value_block += index_block[i].value_size_with_padding;
    }
    // write VERSION key/value
    struct sfo_index *version = &index_buffer[keys_count];
    version->key_offset = keys_offset;
    version->alignment = 4;
    version->data_type = 4;
    version->value_size = 4;
    version->value_size_with_padding = 4;
    version->data_offset = 0;
    version->data_offset = (int)((char *)value_buffer - buffer) - 100;
    strcpy(keys_buffer_start + keys_offset, "VERSION");
    int ver = 0x10000;
    memcpy(value_buffer, &ver, 4);
    // set the SFO to 3 keys
    sfo_data->key_offset = 68;
    sfo_data->value_offset = 100;
    sfo_data->pair_count = 3;
	return SFO_SIZE;
}
