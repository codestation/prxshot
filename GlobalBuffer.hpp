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
