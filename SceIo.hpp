/*
 * SceIo.h
 *
 *  Created on: 06/05/2011
 *      Author: code
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
};

#endif /* SCESTREAM_H_ */
