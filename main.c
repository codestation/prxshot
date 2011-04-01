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
#include "minIni.h"
#include "logger.h"

PSP_MODULE_INFO("prxshot", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(8);


#define PICTURE_DIR_MS "ms0:/PSP/SCREENSHOT"
#define PICTURE_DIR_GO "ef0:/PSP/SCREENSHOT"
#define GAMEID_DIR "disc0:/UMD_DATA.BIN"
#define SHOT_BLK_NAME "shot_blk"
#define MAX_IMAGES 10000
#define BMP_SIZE 391734

char eboot_path[128];
char ini_path[128];
char imagefile[64];
char directory[32];
char picture[32];

SceUID last_id = 0;
int game_found = 0;
int clear_cache = -1;
int sema_wait = 0;
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
		sprintf(buffer, picture, directory, last_id++);
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
    // wait for the sema signal so the game is loaded
    if(sema_wait) {
        sceKernelWaitSema(sema, 1, NULL);
        sema_wait = 0;
    }
    // check if an UMD (or ISO) is present
    kprintf("Trying to open %s\n", GAMEID_DIR);
	SceUID fd = sceIoOpen(GAMEID_DIR, PSP_O_RDONLY, 0777);
	if(fd >= 0) {
	    kprintf("UMD/ISO found\n");
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
	            if(*eboot_path) {
                    if(generate_gameid(eboot_path, gameid, sizeof(gameid))) {
                        strcpy(buffer, gameid);
                        game_found = 1;
                    } else {
                        kprintf("Cannot generate gameid, defaulting to Homebrew\n");
                        strcpy(buffer, "Homebrew");
                    }
	            }
	        } else {
	            kprintf("Boot from disc and no SFO found? o.O, defaulting to Homebrew\n");
	            strcpy(buffer, "Homebrew");
	        }
	    }
	}
}

void create_gamedir(char *buffer, const char *argp) {
    int model = sceKernelGetModel();
    int force_ms0 = 0;
    if(model == PSP_MODEL_GO) {
        force_ms0 = ini_getbool("General", "PSPGoUseMS0", 0, ini_path);
    }
    strcpy(buffer, model == PSP_MODEL_GO ? PICTURE_DIR_GO : PICTURE_DIR_MS);
    if(force_ms0) {
        //Make sure that the /PSP directory exists first
        kprintf("PSPGoUseMS0 enabled, forcing ms0\n");
        sceIoMkdir("ms0:/PSP", 0777);
        memcpy(buffer, "ms0", 3);
    }
    sceIoMkdir(buffer, 0777);
    strcat(buffer,"/");
    get_gameid(buffer + strlen(model == PSP_MODEL_GO ? PICTURE_DIR_GO : PICTURE_DIR_MS)+1);
    kprintf("Creating directory %s\n", buffer);
    sceIoMkdir(buffer, 0777);
}

// this causes problems to game categories D:
void update_xmb_cache() {
    if(clear_cache < 0) {
        clear_cache = ini_getbool("General", "XMBClearCache", 0, ini_path);
    }
    if(clear_cache && sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH) {
        sceIoDevctl("fatms0:", 0x0240D81E, NULL, 0, NULL, 0);
    }
}

int pbp_thread_start(SceSize args, void *argp) {
    kprintf("pbp_thread_start called\n");
    char *str = eboot_path[0] == 0 ? NULL : eboot_path;
    write_pbp(directory, str, argp);
    // refresh the cache after creating the PSCM.DAT
    update_xmb_cache();
    return 0;
}

// function to be called into the module_start code, it saves the argp
// and creates a payload in the user stack
void syscall_save_argp(int args, const char *argp, void *user_stack) {
    int k1 = pspSdkSetK1(0);
    if(argp && args <= sizeof(eboot_path)) {
        kprintf("Saving path: %s\n", argp);
        memcpy(eboot_path, argp, args);
        eboot_path[args] = 0;
    } else {
        eboot_path[0] = 0;
    }
    if(user_stack)
        create_stack_payload(user_stack);
    sceKernelSignalSema(sema, 1);
    pspSdkSetK1(k1);
}

int thread_start(SceSize args, void *argp) {
    // read config file
    kprintf("PRXshot main thread started\n");
    create_path(ini_path, argp, "prxshot.ini");
    int key_button = ini_getlhex("General", "ScreenshotKey", PSP_CTRL_NOTE, eboot_path);
    kprintf("Read ScreenshotKey: %08X\n", key_button);
    ini_gets("General", "ScreenshotName", "%s/pic_%04d.bmp", picture, sizeof(picture), eboot_path);
    kprintf("Read ScreenshotName: %s\n", picture);
    // clear buffer
    memset(eboot_path, 0, sizeof(eboot_path));
    if(sceKernelInitKeyConfig() != PSP_INIT_KEYCONFIG_VSH && sceKernelBootFrom() == PSP_BOOT_MS) {
        kprintf("Booting from Memory Stick/Internal Storage\n");
        hook_module_start();
        sema_wait = 1;
        sema = sceKernelCreateSema("hook-sema", 0, 0, 1, NULL);
    }
	int picture_id = 0;
	int directory_created = 0;
	int pbp_created = 0;
	kprintf("Entering screenshot loop\n");
	while(picture_id >= 0) {
		SceCtrlData pad;
		sceCtrlPeekBufferPositive(&pad, 1);
		if(pad.Buttons) {
			if((pad.Buttons & key_button) == key_button) {
				if(!directory_created) {
					create_gamedir(directory, argp);
					picture_id = update_filename(directory, imagefile);
					directory_created = 1;
				} else {
				    // recreate the XMB screenshot directory if is deleted
				    if(sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH) {
                        SceUID dfd = sceIoDopen(directory);
                        if(dfd >= 0) {
                            sceIoDclose(dfd);
                        } else {
                            sceIoMkdir(directory, 0777);
                            // create the PSCM.DAT again after the screenshot
                            pbp_created = 0;
                        }
				    }
				}
				kprintf("Taking shot\n");
				if(take_shot(imagefile) == 0) {
				    kprintf("Screenshot OK\n");
				} else {
				    kprintf("Screenshot fail\n");
				}
				//update the filename and wait for the next shot
				picture_id = update_filename(directory, imagefile);
				//launch a thread after the first shot to create the PSCM.DAT
				if(!pbp_created) {
				    SceUID thid = sceKernelCreateThread("pbp_thread", pbp_thread_start, 0x20, 4096, 0, 0);
				    if(thid >= 0) {
				        sceKernelStartThread(thid, args, argp);
				    }
				    pbp_created = 1;
				} else {
				    update_xmb_cache();
				}
			}
		}
		sceKernelDelayThread(100000); //1.000.000 = 1 seg
	}
	return 0;
}

int module_start(SceSize argc, void *argp) {
    kprintf(">>>>>>>>>>>>>>>>>\nPRXshot started\n");
	SceUID thid = sceKernelCreateThread("prxshot", thread_start, 0x10, 4096, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, argc, argp);
	return 0;
}

int module_stop(SceSize argc, void *argp) {
	return 0;
}
