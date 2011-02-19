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

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspthreadman.h>
#include <pspsuspend.h>
#include <pspctrl.h>
#include <pspinit.h>
#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "pbp.h"
#include "logger.h"

PSP_MODULE_INFO("ScreenPRX", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(0);

#define PSP_MEMORY_PARTITION_UMDCACHE 8
#define PICTURE_DIR_MS "ms0:/PSP/SCREENSHOT/"
#define PICTURE_DIR_GO "ef0:/PSP/SCREENSHOT/"
#define GAMEID_DIR "disc0:/UMD_DATA.BIN"
#define MAX_IMAGES 10000
#define BMP_SIZE 391734
#define MODEL_SLIM 1
#define MODEL_PSPGO 4

char *sceKernelGetUMDData(void);
int sceKernelGetModel();

SceUID thid;
SceUID last_id = 0;
char directory[24];
char imagefile[64];

SceUID block_id = -1;
void *block_addr = NULL;
int volatile_size = 0;
void *volatile_addr = NULL;
int game_found = 0;
int model = -1;
int api = -1;
int boot_from = -1;
const char *eboot_path = NULL;

void *kalloc(SceSize size, int type) {
	block_id = sceKernelAllocPartitionMemory(type, "shot-prx", PSP_SMEM_Low, size, NULL);
	if(block_id >= 0)
		block_addr = sceKernelGetBlockHeadAddr(block_id);
	return block_addr;
}

void kfree(void *addr) {
    if(volatile_addr) {
        sceKernelVolatileMemUnlock(0);
        volatile_addr = NULL;
        return;
    }
    if(block_addr == addr) {
		sceKernelFreePartitionMemory(block_id);
		block_id = -1;
		block_addr = NULL;
	}
}

void *get_mem(SceSize size) {
    void *mem = NULL;
    if((game_found || api == PSP_INIT_KEYCONFIG_VSH) && model >= MODEL_SLIM) {
        // use the umd cache only if is a game and slim or superior
        logger("game found, getting memory from umd cache\n");
        mem = kalloc(size, PSP_MEMORY_PARTITION_UMDCACHE);
    }
    if(!mem) {
        // else get the memory from kernel
        logger("getting memory from kernel\n");
        mem = kalloc(size, PSP_MEMORY_PARTITION_KERNEL);
        if(!mem && api == PSP_INIT_KEYCONFIG_GAME) {
            // as a last resort, use the volatile mem
            logger("getting memory from volatile\n");
            if(!sceKernelVolatileMemTryLock(0, &volatile_addr, &volatile_size)) {
                if(volatile_size < BMP_SIZE) {
                    sceKernelVolatileMemUnlock(0);
                    volatile_addr = NULL;
                    logger("fail memory from volatile\n");
                } else {
                    logger("success memory from volatile\n");
                }
                return volatile_addr;
            } else {
                volatile_addr = NULL;
            }
        } else {
            logger("failed getting memory from kernel\n");
        }
    }
	return mem;
}

int take_shot(const char *path) {
	void *frame_addr;
	int frame_width, pixel_format;
	unsigned int ptr;
	void *mem = get_mem(BMP_SIZE);
	if(mem) {
		sceDisplayWaitVblankStart();
		if(sceDisplayGetFrameBuf(&frame_addr, &frame_width, &pixel_format, PSP_DISPLAY_SETBUF_NEXTFRAME) >= 0 && frame_addr) {
			ptr = (unsigned int)frame_addr;
			ptr |= ptr & 0x80000000 ?  0xA0000000 : 0x40000000;
			bitmapWrite((void *)ptr, mem, pixel_format, path);
			kfree(mem);
			return 0;
		}
	}
	return -1;
}

int update_filename(const char *directory, char *buffer) {
	int end = last_id;
	do {
		sprintf(buffer, "%s/pic_%04d.bmp", directory, last_id++);
		if(last_id == MAX_IMAGES)
			last_id = 0;
		SceUID fd = sceIoOpen(buffer, PSP_O_RDONLY, 0777);
		if(fd < 0) {
			return last_id;
		}
		sceIoClose(fd);
	} while(end != last_id);
	return -1;
}

const char *get_eboot_path() {
    return (const char *)(0x48800000 + 0x17FFAD0);   //argv[0]
    //return (const char *)(0x48800000 + 0x17FFEC0); //homebrew
}

void get_gameid(char *buffer) {
    char gameid[10];
	SceUID fd = sceIoOpen(GAMEID_DIR, PSP_O_RDONLY, 0777);
	if(fd >= 0) {
		game_found = 1;
		sceIoRead(fd, gameid, 10);
		sceIoClose(fd);
		strcpy(buffer, gameid);
		if(gameid[4] == '-')
		    strcpy(buffer + 4, gameid + 5);
	} else {
	    if(api == PSP_INIT_KEYCONFIG_VSH) {
	        strcpy(buffer,"VSH");
	    }else {
	        if(boot_from == PSP_BOOT_MS) {
	            eboot_path = get_eboot_path();
	            if(!memcmp(eboot_path, PICTURE_DIR_MS, 4) ||
	               !memcmp(eboot_path, PICTURE_DIR_GO, 4)) {
	                if(read_gameid(eboot_path, gameid, sizeof(gameid))) {
	                    strcpy(buffer, gameid);
	                    game_found = 1;
	                } else {
	                    strcpy(buffer, "Homebrew");
	                }
	            }
	        }
	    }
	}
}

void create_gamedir(char *buffer) {
	strcpy(buffer, model == MODEL_PSPGO ? PICTURE_DIR_GO : PICTURE_DIR_MS);
	get_gameid(buffer + strlen(model == MODEL_PSPGO ? PICTURE_DIR_GO : PICTURE_DIR_MS));
	sceIoMkdir(buffer, 0777);
}

int pbp_thread_start(SceSize args, void *argp) {
    write_pbp(directory, eboot_path);
    logger("pbp thread end\n");
    return 0;
}

int thread_start(SceSize args, void *argp) {
	int id = 0;
	int init = 0;
	int created = 0;
	model = sceKernelGetModel();
	api = sceKernelInitKeyConfig();
	boot_from = sceKernelBootFrom();
	while(id >= 0) {
		SceCtrlData pad;
		sceCtrlPeekBufferPositive(&pad, 1);
		if(pad.Buttons) {
			if(pad.Buttons & PSP_CTRL_NOTE) {
				if(!init) {
					create_gamedir(directory);
					id = update_filename(directory, imagefile);
					init = 1;
				}
				take_shot(imagefile);
				//update the filename and wait for the next shot
				id = update_filename(directory, imagefile);
				//launch a thread after the first shot to create the PSCM.DAT
				if(!created && api != PSP_INIT_KEYCONFIG_VSH) {
				    SceUID thid = sceKernelCreateThread("pbp_thread", pbp_thread_start, 0x20, 0x1000, 0, 0);
				    if(thid >= 0)
				        sceKernelStartThread(thid, 0, 0);
				    created = 1;
				}
			}
		}
		sceKernelDelayThread(100000); //1.000.000 = 1 seg
	}
	return 0;
}

int module_start(SceSize argc, void *argp) {
	SceUID thid = sceKernelCreateThread("ScreenPRX", thread_start, 0x10, 0x1000, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	return 0;
}

int module_stop(SceSize argc, void *argv) {
	return 0;
}
