/*
 * PspHandler.h
 *
 *  Created on: 07/05/2011
 *      Author: code
 */

#ifndef PSPHANDLER_H_
#define PSPHANDLER_H_

// pspinit.h doesn't have C style exports in C++ ¬_¬
#ifdef __cplusplus
extern "C" {
#endif
#include <pspinit.h>
#ifdef __cplusplus
}
#endif
#include "pspdefs.h"

class PspHandler {
    enum game_type {HOMEBREW, UMD_ISO, XMB, PSN, PSX};

    //static variables
    static STMOD_HANDLER previous;
    static volatile int loader_found;
    static bool module_found;
    static bool eboot_found;
    game_type type;
    char *pbp_path;
    // string array
    static const char *blacklist[];
    //functions
    static int module_start_handler(SceModule2 *module);
    static bool checkBlacklist(const char *str);
public:
    enum boot_type {FLASH = PSP_BOOT_FLASH,
                    DISC = PSP_BOOT_DISC,
                    MS = PSP_BOOT_MS };

    enum app_type { VSH = PSP_INIT_KEYCONFIG_VSH,
                    GAME = PSP_INIT_KEYCONFIG_GAME,
                    POPS = PSP_INIT_KEYCONFIG_POPS};

    enum model_type {MODEL_PHAT, MODEL_SLIM, MODEL_GO = 4};

    PspHandler();
    inline int updated() { return loader_found ? loader_found-- : 0; }
    inline const char *getPBPPath() { return pbp_path; }
    inline game_type getGameType() { return type; }
    inline boot_type bootFrom() { return static_cast<boot_type>(sceKernelBootFrom()); }
    inline app_type applicationType() { return static_cast<app_type>(sceKernelInitKeyConfig()); }
    inline model_type getModel() { return static_cast<model_type>(sceKernelGetModel()); }
    int getKeyPress();
    bool isPressed(unsigned int buttons);
    ~PspHandler();
};

#endif /* PSPHANDLER_H_ */
