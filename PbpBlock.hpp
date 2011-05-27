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

#ifndef PBPBLOCK_H_
#define PBPBLOCK_H_

#include <stddef.h>
#include "Thread.hpp"
#include "SceIo.hpp"
#include "SfoBlock.hpp"

#define BUFFER_SIZE 12288 //12KiB

class PbpBlock : public Thread {
public:
    typedef void (* THREAD_CALLBACK)(void);
    PbpBlock();
    ~PbpBlock();
    PbpBlock(const char *file);
    bool load();
    void setSfoPath(const char *path);
    void outputDir(const char *path);
    inline void onStop(THREAD_CALLBACK thread_cb) {stop_func = thread_cb; }
    inline void reset() { is_created = false; };
    //FIXME
    inline bool created() { return is_created; }
    inline SfoBlock *getSFO() { return sfo; }
protected:
    int run();
private:
    struct pbp_header {
        char id[4];
        unsigned int version;
        unsigned int sfo_offset;
        unsigned int icon0_offset;
        unsigned int icon1_offset;
        unsigned int pic0_offset;
        unsigned int pic1_offset;
        unsigned int snd0_offset;
        unsigned int psp_offset;
        unsigned int psar_offset;
    }__attribute__((packed));

    enum file_type {FILE_PBP, FILE_SFO};
    THREAD_CALLBACK stop_func;
    char *file;
    const char *sfo_path;
    char *outfile;
    pbp_header *header;
    SfoBlock *sfo;
    bool is_created;
    void init(const char *path);
    void appendData(SceIo *out, SceIo *in, size_t size);
    SfoBlock *generatePSCM(SfoBlock *sfo);

};

#endif /* PBPBLOCK_H_ */
