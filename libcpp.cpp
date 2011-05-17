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

#include <stddef.h>
#include <malloc.h>
#include <string.h>

// error handler for pure virtual functions
extern "C" void __cxa_pure_virtual() { while (1); }

void* __dso_handle = (void*) &__dso_handle;

void *operator new(size_t size) {
	return malloc(size);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void *ptr) {
	free(ptr);
}

void operator delete[](void *ptr) {
    free(ptr);
}

char *strdup(const char *s1) {
    char *s2 = (char *)malloc(strlen(s1)+1);
    if(s2 == NULL) {
        return NULL;
    }
    strcpy(s2, s1);
    return s2;
}

char *strjoin(const char *s1, const char *s2) {
    char *s3 = (char *)malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(s3, s1);
    return strcat(s3, s2);
}
