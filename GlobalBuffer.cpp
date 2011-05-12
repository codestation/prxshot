/*
 * GlobalBuffer.cpp
 *
 *  Created on: 09/05/2011
 *      Author: code
 */

#include <pspsysmem.h>
#include <pspmodulemgr.h>
#include <pspsuspend.h>
// pspinit.h doesn't have C style exports in C++ ¬_¬
#ifdef __cplusplus
extern "C" {
#endif
#include <pspinit.h>
#ifdef __cplusplus
}
#endif
#include "GlobalBuffer.h"
#include "pspdefs.h"
#include "logger.h"

#define GLB_BLK_NAME "kallocBlk"

GlobalBuffer::GlobalBuffer(): mem_id(-1), use_volatile(false) {}

void *GlobalBuffer::alloc(int size, mode_type mode) {
    void *ptr = NULL;
    int api = sceKernelInitKeyConfig();
    if((mode == MODE_GAME || api == PSP_INIT_KEYCONFIG_VSH) && sceKernelGetModel() >= PSP_MODEL_SLIM) {
        // use the umd cache only if is a game and slim or superior
        kprintf("Allocating %i bytes from umd cache\n", size);
        mem_id = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_UMDCACHE, GLB_BLK_NAME, PSP_SMEM_Low, size, NULL);
    }
    if(mem_id < 0) {
        // else get the memory from kernel
        kprintf("Allocating %i bytes from kernel memory\n", size);
        mem_id = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_KERNEL, GLB_BLK_NAME, PSP_SMEM_Low, size, NULL);
        if(mem_id < 0 && api == PSP_INIT_KEYCONFIG_GAME) {
            // as a last resort, use the volatile mem
            kprintf("Allocating block from volatile\n");
            if(!sceKernelVolatileMemTryLock(0, &ptr, &size)) {
                kprintf("Volatile alloc OK\n");
                use_volatile = true;
            }
        }
    }
    return (mem_id >= 0) ? sceKernelGetBlockHeadAddr(mem_id) : ptr;
}

GlobalBuffer::~GlobalBuffer() {
    if(use_volatile) {
        sceKernelVolatileMemUnlock(0);
    } else if(mem_id >= 0) {
        sceKernelFreePartitionMemory(mem_id);
    }
}
