/*
 * PbpBlock.h
 *
 *  Created on: 04/05/2011
 *      Author: code
 */

#ifndef PBPBLOCK_H_
#define PBPBLOCK_H_

#include <stddef.h>
#include "Thread.hpp"
#include "SceIo.hpp"
#include "SfoBlock.hpp"

#define PARENTAL_LEVEL "PARENTAL_LEVEL"
#define VERSION "VERSION"
#define TITLE "TITLE"
#define GAME_ID "GAME_ID"

#define SFO_PATH "disc0:/PSP_GAME/PARAM.SFO"
#define ICON0_PATH "disc0:/PSP_GAME/ICON0.PNG"
#define PIC1_PATH "disc0:/PSP_GAME/PIC1.PNG"

#define BUFFER_SIZE 12288 //12KiB

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

class PbpBlock : public Thread {
    char *file;
    char *outfile;
    pbp_header *header;
    SfoBlock *sfo;
    bool is_created;
    void init(const char *path);
    void appendData(SceIo *out, SceIo *in, size_t size);
    SfoBlock *generatePSCM(SfoBlock *sfo);
    enum file_type {FILE_PBP, FILE_SFO};
protected:
    int run();
public:
    PbpBlock();
    PbpBlock(const char *file);
    bool load();
    void outputDir(const char *path);
    void unload();
    //FIXME
    bool created() { return is_created; }
    SfoBlock *getSFO() { return sfo; }
    ~PbpBlock();
};

#endif /* PBPBLOCK_H_ */
