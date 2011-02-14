/*
 * pbp.h
 *
 *  Created on: 13/02/2011
 *      Author: code
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

void write_pbp(const char *path);

#endif /* PBP_H_ */
