#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspsuspend.h>
#include <pspinit.h>
#include <stddef.h>
#include "pspdefs.h"
#include "kmalloc.h"
#include "logger.h"

#define DEFAULT_KMALLOC_HEAP_SIZE_KB 64

#define KALLOC_BLK_NAME "kallocBlk"

extern int kmalloc_heap_kb_size __attribute__((weak));

SceUID heap = -1;
SceUID galloc_id = -1;
void *galloc_ptr = NULL;
int volatile_use = 0;

int libc_init() {
	if(!kmalloc_heap_kb_size)
		kmalloc_heap_kb_size = DEFAULT_KMALLOC_HEAP_SIZE_KB;
	kprintf("Creating heap of %i KiB\n", kmalloc_heap_kb_size);
    heap = sceKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, kmalloc_heap_kb_size * 1024, 1, "kmHeap");
    return heap < 0 ? heap : 0;
}

void libc_finish() {
	if(heap >= 0)
		sceKernelDeleteHeap(heap);
}

/*
void *kalloc(SceSize size, const char *name, int *id, int part, int type) {
    void *block = NULL;
    *id = sceKernelAllocPartitionMemory(part, name, type, size + 63, NULL);
    if(*id >= 0)
        block = sceKernelGetBlockHeadAddr(*id);
    return (void *)(((u32)block + 63) & ~63);
}

inline void kfree(int id) {
    sceKernelFreePartitionMemory(id);
}

void *kalloc_volatile() {
    void *block;
    int size;
    return !sceKernelVolatileMemTryLock(0, &block, &size) ? block : NULL;
}

inline void kfree_volatile() {
    sceKernelVolatileMemUnlock(0);
}

void *galloc(size_t size, int mode) {
    void *mem = NULL;
    int api = sceKernelInitKeyConfig();
    if((mode == MODE_GAME || api == PSP_INIT_KEYCONFIG_VSH) && sceKernelGetModel() >= PSP_MODEL_SLIM) {
        // use the umd cache only if is a game and slim or superior
        mem = kalloc(size, KALLOC_BLK_NAME, &galloc_id, PSP_MEMORY_PARTITION_UMDCACHE, PSP_SMEM_Low);
    }
    if(!mem) {
        // else get the memory from kernel
        mem = kalloc(size, KALLOC_BLK_NAME, &galloc_id, PSP_MEMORY_PARTITION_KERNEL, PSP_SMEM_Low);
        if(!mem && api == PSP_INIT_KEYCONFIG_GAME) {
            // as a last resort, use the volatile mem
            mem = kalloc_volatile();
            if(mem)
                volatile_use = 1;
        }
    }
    galloc_ptr = mem;
    return mem;
}

void gfree(void *ptr) {
    if(volatile_use) {
        volatile_use = 0;
        kfree_volatile();
    } else if(ptr == galloc_ptr) {
        galloc_ptr = 0;
        kfree(galloc_id);
        galloc_id = -1;
    }
}
*/
#ifdef DEBUG_MEMORY
int max_memory = 0;
int cur_memory = 0;
#endif

void *malloc(size_t size) {
#ifdef DEBUG_MEMORY
    int free = sceKernelHeapTotalFreeSize(heap);
    kprintf("malloc called, %i bytes\n", size);
    void *ptr = sceKernelAllocHeapMemory(heap, size);
    free = free - sceKernelHeapTotalFreeSize(heap);
    cur_memory += free;
    if(cur_memory > max_memory) {
        max_memory = cur_memory;
        kprintf("Max used memory: %i bytes\n", max_memory);
    }
    return ptr;
#else
	return sceKernelAllocHeapMemory(heap, size);
#endif
}

void free(void *ptr) {
#ifdef DEBUG_MEMORY
    int free = 0;
    if(ptr) free = sceKernelHeapTotalFreeSize(heap);
    free = sceKernelHeapTotalFreeSize(heap) - free;
    cur_memory -= free;
#else
	if(ptr) sceKernelFreeHeapMemory(heap, ptr);
#endif
}
