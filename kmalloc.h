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

#ifndef _KMALLOC_H_
#define _KMALLOC_H_

#include <pspkerneltypes.h>
#include <stddef.h>

#define KMALLOC_HEAP_SIZE_KB(size_kb) \
	int kmalloc_heap_kb_size = (size_kb)

#ifdef __cplusplus
extern "C" {
#endif

int libc_init();
void libc_finish();
void *malloc(size_t size);
void free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif // _KMALLOC_H_
