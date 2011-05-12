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
#include <pspsysmem_kernel.h>
#include <pspsuspend.h>
#include <pspinit.h>
#include <stddef.h>
#include "pspdefs.h"
#include "kmalloc.h"
#include "logger.h"

#define DEFAULT_KMALLOC_HEAP_SIZE_KB 64

extern int kmalloc_heap_kb_size __attribute__((weak));

SceUID heap = -1;

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
