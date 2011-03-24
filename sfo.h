/*
 *  Screenshot module
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
int read_sfo_title(SceUID fd, char *buffer, int size, char *id_buf, int id_size);

#endif /* SFO_H_ */
