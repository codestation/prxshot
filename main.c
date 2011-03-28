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

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspthreadman.h>
#include <pspctrl.h>
#include <pspinit.h>

#include <stdio.h>
#include <string.h>

#include "pspdefs.h"
#include "kalloc.h"
#include "bitmap.h"
#include "pbp.h"
#include "payload.h"
#include "hook_module.h"
#include "logger.h"

PSP_MODULE_INFO("prxshot", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(8);


#define PICTURE_DIR_MS "ms0:/PSP/SCREENSHOT/"
#define PICTURE_DIR_GO "ef0:/PSP/SCREENSHOT/"
#define GAMEID_DIR "disc0:/UMD_DATA.BIN"
#define SHOT_BLK_NAME "shot_blk"
#define MAX_IMAGES 10000
#define BMP_SIZE 391734

char eboot_path[128];
char imagefile[64];
char directory[32];

SceUID last_id = 0;
int game_found = 0;
SceUID sema = 0;

void *get_mem(SceSize size, int *id) {
    void *mem = NULL;
    int api = sceKernelInitKeyConfig();
    if((game_found || api == PSP_INIT_KEYCONFIG_VSH) && sceKernelGetModel() >= PSP_MODEL_SLIM) {
        // use the umd cache only if is a game and slim or superior
        mem = kalloc(size, SHOT_BLK_NAME, id, PSP_MEMORY_PARTITION_UMDCACHE, PSP_SMEM_Low);
    }
    if(!mem) {
        // else get the memory from kernel
        mem = kalloc(size, SHOT_BLK_NAME, id, PSP_MEMORY_PARTITION_KERNEL, PSP_SMEM_Low);
        if(!mem && api == PSP_INIT_KEYCONFIG_GAME) {
            // as a last resort, use the volatile mem
            mem = kalloc_volatile();
        }
    }
	return mem;
}

int take_shot(const char *path) {
	void *frame_addr;
	SceUID block_id = -1;
	int frame_width, pixel_format;
	unsigned int ptr;
	void *mem = get_mem(BMP_SIZE, &block_id);
	if(mem) {
		sceDisplayWaitVblankStart();
		if(sceDisplayGetFrameBuf(&frame_addr, &frame_width, &pixel_format, PSP_DISPLAY_SETBUF_NEXTFRAME) >= 0 && frame_addr) {
			ptr = (unsigned int)frame_addr;
			ptr |= ptr & 0x80000000 ?  0xA0000000 : 0x40000000;
			bitmapWrite((void *)ptr, mem, pixel_format, path);
			block_id >= 0 ? kfree(block_id) : kfree_volatile();
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

void get_gameid(char *buffer) {
    char gameid[12];
	SceUID fd = sceIoOpen(GAMEID_DIR, PSP_O_RDONLY, 0777);
	if(fd >= 0) {
		game_found = 1;
		sceIoRead(fd, gameid, 10);
		gameid[10] = 0;
		sceIoClose(fd);
		strcpy(buffer, gameid);
		if(gameid[4] == '-')
		    strcpy(buffer + 4, gameid + 5);
	    // sets eboot_path to 0 as it doesn't get initialized on
	    // prxshot_set_argp because is never called for UMD/ISO
	    eboot_path[0] = 0;
	} else {
	    if(sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH) {
	        strcpy(buffer,"XMB");
	    } else {
	        if(sceKernelBootFrom() == PSP_BOOT_MS) {
	            sceKernelWaitSema(sema, 1, NULL);
	            if(*eboot_path) {
                    if(generate_gameid(eboot_path, gameid, sizeof(gameid))) {
                        strcpy(buffer, gameid);
                        game_found = 1;
                    } else {
                        strcpy(buffer, "Homebrew");
                    }
	            }
	        } else {
	            strcpy(buffer, "Homebrew");
	        }
	    }
	}
}

void create_gamedir(char *buffer) {
    int model = sceKernelGetModel();
	strcpy(buffer, model == PSP_MODEL_GO ? PICTURE_DIR_GO : PICTURE_DIR_MS);
	sceIoMkdir(buffer, 0777);
	get_gameid(buffer + strlen(model == PSP_MODEL_GO ? PICTURE_DIR_GO : PICTURE_DIR_MS));
	sceIoMkdir(buffer, 0777);
}

int pbp_thread_start(SceSize args, void *argp) {
    char *str = eboot_path[0] == 0 ? NULL : eboot_path;
    write_pbp(directory, str, argp);
    return 0;
}

// function to be called into the module_start code, it saves the argp
// and creates a payload in the user stack
void syscall_save_argp(int args, const char *argp, void *user_stack) {
    int k1 = pspSdkSetK1(0);
    if(args <= sizeof(eboot_path))
        memcpy(eboot_path, argp, args);
    else
        eboot_path[0] = 0;
    create_stack_payload(user_stack);
    sceKernelSignalSema(sema, 1);
    pspSdkSetK1(k1);
}

int thread_start(SceSize args, void *argp) {
    if(sceKernelInitKeyConfig() != PSP_INIT_KEYCONFIG_VSH && sceKernelBootFrom() == PSP_BOOT_MS) {
        hook_module_start();
        sema = sceKernelCreateSema("hook-sema", 0, 0, 1, NULL);
    }
	int id = 0;
	int init = 0;
	int created = 0;
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
				if(!created) {
				    SceUID thid = sceKernelCreateThread("pbp_thread", pbp_thread_start, 0x20, 4096, 0, 0);
				    if(thid >= 0) {
				        sceKernelStartThread(thid, args, argp);
				    }
				    created = 1;
				}
			}
		}
		sceKernelDelayThread(100000); //1.000.000 = 1 seg
	}
	return 0;
}

int module_start(SceSize argc, void *argp) {
	SceUID thid = sceKernelCreateThread("prxshot", thread_start, 0x10, 4096, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, argc, argp);
	return 0;
}

int module_stop(SceSize argc, void *argp) {
	return 0;
}
