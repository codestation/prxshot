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

#include <pspiofilemgr.h>
#include <pspinit.h>
#include <string.h>
#include "kalloc.h"
#include "pbp.h"
#include "sfo.h"
#include "minIni.h"
#include "logger.h"

#define BUFFER_SIZE 4096

int buffer_id = -1;
void *buffer = NULL;

int generate_gameid(const char *path, char *id_buf, int id_size) {
    struct pbp pbp_data;
    char title[128];
    int res = 0;
    if(!buffer)
        buffer = kalloc(BUFFER_SIZE, "pbp_blk", &buffer_id, PSP_MEMORY_PARTITION_KERNEL, PSP_SMEM_Low);
    if(!buffer)
        return 0;
    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
    if(fd >= 0) {
        sceIoRead(fd, &pbp_data, sizeof(struct pbp));
        int size = pbp_data.icon0_offset - pbp_data.sfo_offset;
        res = read_sfo_title(fd, buffer, size, title, sizeof(title));
        if(res > 0) {
            // Just because all the Homebrew comes with the GAMEID of LocoRoco
            // we need a way to identity one homebrew from another.
            char digest[20];
            sceKernelUtilsSha1Digest((u8 *)title, (u32)res, (u8 *)digest);
            sprintf(buffer, "PS%08X", *(u32 *)digest);
            memcpy(id_buf, buffer, id_size-1);
            id_buf[id_size-1] = 0;
        }
        sceIoClose(fd);
    }
    return res;
}

SceSize append_file(const char *path, SceUID fd, SceUID fdin, int imagesize) {
    int read = BUFFER_SIZE;
    SceSize size = 0;
    SceUID ifd = fdin >= 0 && imagesize > 0 ? fdin : sceIoOpen(path, PSP_O_RDONLY, 0777);
    if(fd >= 0) {
        if(fdin < 0 || fdin != ifd) {
            size = sceIoLseek32(ifd, 0, PSP_SEEK_END);
            sceIoLseek32(ifd, 0, PSP_SEEK_SET);
            imagesize = size;
        } else {
            size = imagesize;
        }
        while(imagesize) {
            read = sceIoRead(ifd, buffer, read);
            if(read <= 0)
                break;
            sceIoWrite(fd, buffer, read);
            imagesize -= read;
            if(imagesize < BUFFER_SIZE) {
                read = imagesize;
            } else {
                read = BUFFER_SIZE;
            }
        }
        if(fdin < 0) {
            sceIoClose(ifd);
        }
    }
    return size;
}

void *create_path(void *buffer, const char *argp, const char *file) {
    strcpy(buffer, argp);
    strrchr(buffer, '/')[1] = 0;
    strcpy(buffer + strlen(buffer), file);
    return buffer;
}

void write_pbp(const char *path, const char *eboot, void *argp) {
    if(!buffer)
        buffer = kalloc(BUFFER_SIZE + 63, "pbp_blk", &buffer_id, PSP_MEMORY_PARTITION_KERNEL, PSP_SMEM_Low);
    if(!buffer)
        return;
    struct pbp pbp_data;
    memset(&pbp_data, 0, sizeof(struct pbp));
    char pbpname[48];
    SceUID sfo_fd;
    strcpy(pbpname, path);
    strcat(pbpname,"/PSCM.DAT");
    SceUID pbp_fd = sceIoOpen(pbpname, PSP_O_RDWR | PSP_O_CREAT | PSP_O_EXCL, 0777);
    if(pbp_fd < 0)
        return;
    int api = sceKernelInitKeyConfig();
    if(eboot) {
        sfo_fd = sceIoOpen(eboot, PSP_O_RDONLY, 0777);
    } else {
        if(api == PSP_INIT_KEYCONFIG_VSH) {
            create_path(buffer, argp, "xmb.sfo");
            sfo_fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
        } else {
            sfo_fd = sceIoOpen(SFO_PATH, PSP_O_RDONLY, 0777);
        }
    }
    if(sfo_fd < 0) {
        return;
    }
    SceSize size;
    if(eboot) {
        sceIoRead(sfo_fd, &pbp_data, sizeof(struct pbp));
        size = pbp_data.icon0_offset - pbp_data.sfo_offset;
    } else {
        size = sceIoLseek32(sfo_fd, 0, PSP_SEEK_END);
        sceIoLseek32(sfo_fd, 0, PSP_SEEK_SET);
    }
    // create SFO data
    if(size > (BUFFER_SIZE - SFO_SIZE)) {
        //kprintf("SFO size is too big: %i bytes\n", size);
        return;
    }
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
    sceIoWrite(pbp_fd, buffer, size);
    if(eboot) {
        sceIoLseek32(sfo_fd, pbp_data.icon0_offset, PSP_SEEK_SET);
    }
    int imgsize = pbp_data.icon1_offset - pbp_data.icon0_offset;
    if((!eboot || pbp_data.icon0_offset != pbp_data.icon1_offset) && api != PSP_INIT_KEYCONFIG_VSH) {
        strcpy(buffer, ICON0_PATH);
    } else {
        create_path(buffer, argp, "default_icon0.png");
    }
    pbp_data.icon0_offset = pbp_data.sfo_offset + size;
    // write ICON0.PNG
    size = append_file(buffer, pbp_fd, sfo_fd, imgsize);

    pbp_data.icon1_offset = pbp_data.icon0_offset + size;
    pbp_data.pic0_offset = pbp_data.icon0_offset + size;
    if(eboot) {
        sceIoLseek32(sfo_fd, pbp_data.pic1_offset, PSP_SEEK_SET);
    }
    create_path(buffer, argp, "prxshot.ini");
    int pic1 = ini_getbool("General", "CreatePic1", 1, buffer);
    if(pic1) {
        imgsize = pbp_data.snd0_offset - pbp_data.pic1_offset;
        if((!eboot || pbp_data.pic1_offset != pbp_data.snd0_offset)  && api != PSP_INIT_KEYCONFIG_VSH) {
            strcpy(buffer, PIC1_PATH);
        } else {
            if(api == PSP_INIT_KEYCONFIG_VSH) {
                create_path(buffer, argp, "xmb_pic1.png");
            } else {
                create_path(buffer, argp, "default_pic1.png");
            }
        }
    }
    pbp_data.pic1_offset = pbp_data.icon0_offset + size;
    // write PIC1.PNG
    if(pic1) {
        size = append_file(buffer, pbp_fd, sfo_fd, imgsize);
    } else {
        size = 0;
    }
    kfree(buffer_id);
    pbp_data.snd0_offset = pbp_data.pic1_offset + size;
    pbp_data.psp_offset = pbp_data.pic1_offset + size;
    pbp_data.psar_offset = pbp_data.pic1_offset + size;
    sceIoLseek32(pbp_fd, 0, PSP_SEEK_SET);
    // write updated PBP header
    sceIoWrite(pbp_fd, &pbp_data, sizeof(struct pbp));
    sceIoClose(pbp_fd);
    if(eboot) {
        sceIoClose(sfo_fd);
    }
}
