#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspthreadman.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "pbp.h"
#include "logger.h"

PSP_MODULE_INFO("ScreenPRX", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(0);

#define PICTURE_DIR "ms0:/PSP/SCREENSHOT/"
#define GAMEID_DIR "disc0:/UMD_DATA.BIN"
#define MAX_IMAGES 10000
#define BMP_SIZE 391734

char *sceKernelGetUMDData(void);

SceUID thid;
SceUID last_id = 0;
char directory[24];
char imagefile[64];

SceUID block_id = -1;
void *block_addr = NULL;
int game_found = 0;

void *kalloc(SceSize size, int type) {
	block_id = sceKernelAllocPartitionMemory(type, "shot-prx", PSP_SMEM_Low, size, NULL);
	if(block_id >= 0)
		block_addr = sceKernelGetBlockHeadAddr(block_id);
	return block_addr;
}

void kfree(void *addr) {
	if(block_addr == addr) {
		sceKernelFreePartitionMemory(block_id);
		block_id = -1;
		block_addr = NULL;
	}
}

void *get_mem(SceSize size) {
	if(!game_found) {
		return kalloc(size, 1);
	} else {
		void *mem = kalloc(size, 8);
		if(!mem) {
			return kalloc(size, 1);
		}
		return mem;
	}
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
			bitmapWrite((void *)ptr, block_addr, pixel_format, path);
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

void get_gameid(char *buffer) {
	SceUID fd = sceIoOpen(GAMEID_DIR, PSP_O_RDONLY, 0777);
	if(fd >= 0) {
		game_found = 1;
		sceIoRead(fd, buffer, 4);
		sceIoLseek32(fd, 1, PSP_SEEK_CUR);
		sceIoRead(fd, buffer+4, 5);
		sceIoClose(fd);
		buffer[9] = 0;
		buffer[10] = 0;
	} else {
		strcpy(buffer,"Homebrew");
	}
}

void create_gamedir(char *buffer) {
	strcpy(buffer, PICTURE_DIR);
	get_gameid(buffer + strlen(PICTURE_DIR));
	sceIoMkdir(buffer, 0777);
}

int pbp_thread_start(SceSize args, void *argp) {
    write_pbp(directory);
    return 0;
}

int thread_start(SceSize args, void *argp) {
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
				id = update_filename(directory, imagefile);
				if(!created) {
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

int module_start(SceSize argc, void *argv) {
	SceUID thid = sceKernelCreateThread("ScreenPRX", thread_start, 0x10, 0x1000, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	return 0;
}

int module_stop(SceSize argc, void *argv) {
	return 0;
}
