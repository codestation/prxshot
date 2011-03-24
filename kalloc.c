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

#include "kalloc.h"

// get a block of memory 64-byte aligned
void *kalloc(SceSize size, const char *name, int *id, int part, int type) {
    void *block = NULL;
    *id = sceKernelAllocPartitionMemory(part, name, type, size + 63, NULL);
    if(*id >= 0)
        block = sceKernelGetBlockHeadAddr(*id);
    return (void *)(((u32)block + 63) & ~63);
}
void kfree(int id) {
    sceKernelFreePartitionMemory(id);
}

void *kalloc_volatile() {
    void *block;
    int size;
    return !sceKernelVolatileMemTryLock(0, &block, &size) ? block : NULL;
}

void kfree_volatile() {
    sceKernelVolatileMemUnlock(0);
}
