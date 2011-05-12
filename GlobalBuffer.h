/*
 * GlobalBuffer.h
 *
 *  Created on: 09/05/2011
 *      Author: code
 */

#ifndef GLOBALBUFFER_H_
#define GLOBALBUFFER_H_

#include <pspkerneltypes.h>
#include <stddef.h>

class GlobalBuffer {
    SceUID mem_id;
    bool use_volatile;
public:
    enum mode_type {MODE_GAME, MODE_XMB};
    GlobalBuffer();
    void *alloc(int size, mode_type mode);
    ~GlobalBuffer();
};

#endif /* GLOBALBUFFER_H_ */
