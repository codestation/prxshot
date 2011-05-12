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

#ifndef SCESTREAM_H_
#define SCESTREAM_H_

#include <pspiofilemgr.h>

class SceIo {
    SceUID fd;
public:
    enum open_mode {FILE_READ = PSP_O_RDONLY,
                    FILE_WRITE = PSP_O_WRONLY,
                    FILE_RDWR = PSP_O_RDWR | PSP_O_CREAT | PSP_O_EXCL};
    enum seek_mode {FILE_SET = PSP_SEEK_SET,
                    FILE_CUR = PSP_SEEK_CUR,
                    FILE_END = PSP_SEEK_END };
    SceIo() {}

    inline bool open(const char *file, open_mode mode) {
        return (fd = sceIoOpen(file, mode, 0777)) >= 0 ? true : false;
    }
    inline void close() {
        sceIoClose(fd);
    }

    inline int seek(int offset, seek_mode mode) {
        return sceIoLseek32(fd, offset, mode);
    }

    inline void rewind() {
        sceIoLseek32(fd, 0, PSP_SEEK_SET);
    }

    int size() {
        int size = sceIoLseek32(fd, 0, PSP_SEEK_END);
        sceIoLseek32(fd, 0, PSP_SEEK_SET);
        return size;
    }
    inline int read(void *data, int size) {
        return sceIoRead(fd, data, size);
    }

    inline int write(void *data, int size) {
        return sceIoWrite(fd, data, size);
    }

    inline static int mkdir(const char *dir) {
        return sceIoMkdir(dir, 0777);
    }

    inline static bool exists(const char *file) {
        SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
        if(fd < 0)
            return false;
        else {
            sceIoClose(fd);
            return true;
        }
    }
};

#endif /* SCESTREAM_H_ */
