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

#include <pspiofilemgr.h>
#include <string.h>
#include "pbp.h"
#include "sfo.h"
#include "logger.h"

char buffer[1024];

int read_gameid(const char *path, char *id_buf, int id_size) {
    struct pbp pbp_data;
    int res = 0;
    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
    if(fd >= 0) {
        sceIoRead(fd, &pbp_data, sizeof(struct pbp));
        int size = pbp_data.icon0_offset - pbp_data.sfo_offset;
        res = read_sfo_id(fd, buffer, size, id_buf, id_size);
        sceIoClose(fd);
    }
    return res;
}

SceSize append_file(const char *path, SceUID fd, SceUID fdin, int imagesize) {
    int read = sizeof(buffer);
    SceSize size = 0;
    SceUID ifd = fdin >= 0 ? fdin : sceIoOpen(path, PSP_O_RDONLY, 0777);
    if(fd >= 0) {
        if(fdin < 0) {
            size = sceIoLseek32(ifd, 0, PSP_SEEK_END);
            sceIoLseek32(ifd, 0, PSP_SEEK_SET);
        } else {
            size = imagesize;
        }
        while(imagesize) {
            read = sceIoRead(ifd, buffer, read);
            if(read <= 0)
                break;
            sceIoWrite(fd, buffer, read);
            imagesize -= read;
            if(imagesize < sizeof(buffer)) {
                read = imagesize;
            } else {
                read = sizeof(buffer);
            }
        }
        if(fdin < 0) {
            logger("closing image file\n");
            sceIoClose(ifd);
        }
    }
    return size;
}

void write_pbp(const char *path, const char *eboot) {
    char pbpname[48];
    struct pbp pbp_data;
    SceUID sfo_fd;
    strcpy(pbpname, path);
    strcat(pbpname,"/PSCM.DAT");
    logger("opening PSCM.DAT\n");
    SceUID pbp_fd = sceIoOpen(pbpname, PSP_O_RDWR | PSP_O_CREAT | PSP_O_EXCL, 0777);
    if(pbp_fd < 0)
        return;
    if(eboot) {
        log("opening %s\n", eboot);
        sfo_fd = sceIoOpen(eboot, PSP_O_RDONLY, 0777);
    } else {
        sfo_fd = sceIoOpen(SFO_PATH, PSP_O_RDONLY, 0777);
    }
    if(sfo_fd < 0)
        return;
    SceSize size;
    if(eboot) {
        sceIoRead(sfo_fd, &pbp_data, sizeof(struct pbp));
        size = pbp_data.icon0_offset - pbp_data.sfo_offset;
    } else {
        size = sceIoLseek32(sfo_fd, 0, PSP_SEEK_END);
        sceIoLseek32(sfo_fd, 0, PSP_SEEK_SET);
    }
    log("Size of SFO: %i\n", size);
    // create SFO data
    if(size * 2 > sizeof(buffer))
        return;
    memcpy(pbp_data.id, "\0PBP", 4);
    pbp_data.sfo_offset = sizeof(struct pbp);
    size = read_sfo(sfo_fd, buffer, size);
    if(!eboot) {
        sceIoClose(sfo_fd);
        sfo_fd = -1;
    }
    pbp_data.version = 0x10000;
    // write PBP header
    sceIoWrite(pbp_fd, &pbp_data, sizeof(struct pbp));
    // write SFO data
    log("Writting %i bytes of SFO\n", size);
    sceIoWrite(pbp_fd, buffer, size);
    log("sfo offset: %i\n", pbp_data.sfo_offset);
    if(eboot) {
        log("looking for offset %i\n", pbp_data.icon0_offset);
        sceIoLseek32(sfo_fd, pbp_data.icon0_offset, PSP_SEEK_SET);
    }
    pbp_data.icon0_offset = pbp_data.sfo_offset + size;
    // write ICON0.PNG
    size = append_file(ICON0_PATH, pbp_fd, sfo_fd, pbp_data.icon1_offset - pbp_data.icon0_offset);
    log("Size of icon0.png: %i\n", size);
    pbp_data.icon1_offset = pbp_data.icon0_offset + size;
    pbp_data.pic0_offset = pbp_data.icon0_offset + size;
    if(eboot) {
        sceIoLseek32(sfo_fd, pbp_data.pic1_offset, PSP_SEEK_SET);
    }
    pbp_data.pic1_offset = pbp_data.icon0_offset + size;
    // write PIC1.PNG
    size = append_file(PIC1_PATH, pbp_fd, sfo_fd, pbp_data.snd0_offset - pbp_data.pic1_offset);
    log("Size of pic1.png: %i\n", size);
    pbp_data.snd0_offset = pbp_data.pic1_offset + size;
    pbp_data.psp_offset = pbp_data.pic1_offset + size;
    pbp_data.psar_offset = pbp_data.pic1_offset + size;
    sceIoLseek32(pbp_fd, 0, PSP_SEEK_SET);
    // write updated PBP header
    sceIoWrite(pbp_fd, &pbp_data, sizeof(struct pbp));
    logger("closing file\n");
    sceIoClose(pbp_fd);
    if(eboot) {
        sceIoClose(sfo_fd);
    }
}
