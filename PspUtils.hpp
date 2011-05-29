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

#ifndef PSPUTILS_H_
#define PSPUTILS_H_

// pspinit.h doesn't have C style exports in C++ ¬_¬
#ifdef __cplusplus
extern "C" {
#endif
#include <pspinit.h>
#ifdef __cplusplus
}
#endif
#include "Settings.hpp"
#include "pspdefs.h"

class PspUtils {
public:
    enum boot_type {FLASH = PSP_BOOT_FLASH,
                    DISC = PSP_BOOT_DISC,
                    MS = PSP_BOOT_MS };

    enum app_type { VSH = PSP_INIT_KEYCONFIG_VSH,
                    GAME = PSP_INIT_KEYCONFIG_GAME,
                    POPS = PSP_INIT_KEYCONFIG_POPS};

    enum model_type {MODEL_PHAT, MODEL_SLIM, MODEL_GO = 4};

    PspUtils();
    ~PspUtils();
    inline int updated() {
        return loader_found ? loader_found-- : 0;
    }
    inline const char *getPBPPath() {
        return pbp_path;
    }
    //inline game_type getGameType() { return type; }
    inline boot_type bootFrom() {
        return (state == STATE_LOADER) ? DISC : static_cast<boot_type>(sceKernelBootFrom());
    }
    inline static app_type applicationType() {
        return static_cast<app_type>(sceKernelInitKeyConfig());
    }
    inline static model_type getModel() {
        return static_cast<model_type>(sceKernelGetModel());
    }
    static bool isPressed(int buttons, int mask, unsigned int msecs);
    inline void setKeyTimeout(int msecs) { button_timeout = msecs; }
    int getKeyPress();
    static unsigned int getMilliseconds();
    static void clearCache();
    static char *sha1Key(const char *title);
    bool isPressed(int buttons);

private:
    //enum game_type {HOMEBREW, UMD_ISO, XMB, PSN, PSX};
    enum state_type {STATE_NONE, STATE_GAME, STATE_LOADER};
    static STMOD_HANDLER previous;
    static volatile int loader_found;
    static state_type state;
    static unsigned int button_timeout;
    char *pbp_path;

    static int module_start_handler(SceModule2 *module);
    static bool checkBlacklist(const char *str);
};

#endif /* PSPUTILS_H_ */
