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

#ifndef PBP_H_
#define PBP_H_

#define SFO_PATH "disc0:/PSP_GAME/PARAM.SFO"
#define ICON0_PATH "disc0:/PSP_GAME/ICON0.PNG"
#define PIC1_PATH "disc0:/PSP_GAME/PIC1.PNG"

struct pbp {
    char id[4];
    unsigned int version;
    unsigned int sfo_offset;
    unsigned int icon0_offset;
    unsigned int icon1_offset;
    unsigned int pic0_offset;
    unsigned int pic1_offset;
    unsigned int snd0_offset;
    unsigned int psp_offset;
    unsigned int psar_offset;
}__attribute__((packed));

void write_pbp(const char *path, const char *eboot);
int read_gameid(const char *path, char *id_buf, int id_size);
int generate_gameid(const char *path, char *id_buf, int id_size);

#endif /* PBP_H_ */
