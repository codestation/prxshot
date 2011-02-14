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

SceSize append_file(const char *path, SceUID fd) {
    int read = 0;
    SceSize size = 0;
    SceUID ifd = sceIoOpen(path, PSP_O_RDONLY, 0777);
    if(fd >= 0) {
        size = sceIoLseek32(ifd, 0, PSP_SEEK_END);
        sceIoLseek32(ifd, 0, PSP_SEEK_SET);
        while((read = sceIoRead(ifd, buffer, sizeof(buffer))) > 0) {
            sceIoWrite(fd, buffer, read);
        }
        sceIoClose(ifd);
    }
    return size;
}

void write_pbp(const char *path) {
    char pbpname[48];
    strcpy(pbpname, path);
    strcat(pbpname,"/PSCM.DAT");
    SceUID pbp_fd = sceIoOpen(pbpname, PSP_O_RDWR | PSP_O_CREAT | PSP_O_EXCL, 0777);
    if(pbp_fd < 0)
        return;
    SceUID sfo_fd = sceIoOpen(SFO_PATH, PSP_O_RDONLY, 0777);
    if(sfo_fd < 0)
        return;
    SceSize size = sceIoLseek32(sfo_fd, 0, PSP_SEEK_END);
    sceIoLseek32(sfo_fd, 0, PSP_SEEK_SET);
    // create SFO data
    if(size > (sizeof(buffer) - SFO_SIZE))
        return;
    read_sfo(sfo_fd, buffer, size);
    sceIoClose(sfo_fd);
    struct pbp pbp_data;
    memcpy(pbp_data.id, "\0PBP", 4);
    pbp_data.sfo_offset = sizeof(struct pbp);
    pbp_data.version = 0x10000;
    // write PBP header
    sceIoWrite(pbp_fd, &pbp_data, sizeof(struct pbp));
    // write SFO data
    sceIoWrite(pbp_fd, buffer, SFO_SIZE);
    pbp_data.icon0_offset = pbp_data.sfo_offset + SFO_SIZE;
    // write ICON0.PNG
    size = append_file(ICON0_PATH, pbp_fd);
    pbp_data.icon1_offset = pbp_data.icon0_offset + size;
    pbp_data.pic0_offset = pbp_data.icon0_offset + size;
    pbp_data.pic1_offset = pbp_data.icon0_offset + size;
    // write PIC1.PNG
    size = append_file(PIC1_PATH, pbp_fd);
    pbp_data.snd0_offset = pbp_data.pic1_offset + size;
    pbp_data.psp_offset = pbp_data.pic1_offset + size;
    pbp_data.psar_offset = pbp_data.pic1_offset + size;
    sceIoLseek32(pbp_fd, 0, PSP_SEEK_SET);
    // write updated PBP header
    sceIoWrite(pbp_fd, &pbp_data, sizeof(struct pbp));
    sceIoClose(pbp_fd);
}
