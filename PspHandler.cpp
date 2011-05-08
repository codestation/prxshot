#include <pspctrl.h>
#include <string.h>
#include "PspHandler.hpp"

bool PspHandler::module_found = false;
volatile int PspHandler::loader_found = 0;
bool PspHandler::eboot_found = false;
STMOD_HANDLER PspHandler::previous = NULL;

const char *PspHandler::blacklist[] = {
        "Prometheus",
        "Prometheus_lite",
        "aLoader",
        "OpenIdeaController",
        "ISO Loader Eboot",
        "PLoaderGUI"
};

PspHandler::PspHandler() {
    const char *filename = sceKernelInitFileName();
    pbp_path = new char[strlen(filename)+1];
    strcpy(pbp_path, filename);
    if(applicationType() != VSH && bootFrom() != DISC) {
        previous = sctrlHENSetStartModuleHandler(&module_start_handler);
    }
}

bool PspHandler::checkBlacklist(const char *str) {
    for(unsigned int i = 0; i < sizeof(blacklist) / 4; i++) {
        if(!strcmp(str, blacklist[i])) {
            return true;
        }
    }
    return false;
}

int PspHandler::module_start_handler(SceModule2 *module) {
     if(!module_found) {
         switch(sceKernelInitKeyConfig()) {
         case PSP_INIT_KEYCONFIG_POPS:
             module_found = true;
             break;
         case PSP_INIT_KEYCONFIG_GAME:
             // user module found
             if((module->text_addr & 0x80000000) != 0x80000000) {
                 if(checkBlacklist(module->modname)) {
                     loader_found = 1;
                     module_found = true;
                 } else if(strcmp(module->modname, "sceKernelLibrary")) {
                     module_found = true;
                     eboot_found = true;
                 }
             }
             break;
         default:
             break;
         }
     }
     return previous ? previous(module) : 0;
}

int PspHandler::getKeyPress() {
    SceCtrlData pad;
    sceCtrlPeekBufferPositive(&pad, 1);
    return pad.Buttons;
}

bool PspHandler::isPressed(unsigned int buttons) {
    return (getKeyPress() & buttons) == buttons;
}

PspHandler::~PspHandler() {
    delete[] pbp_path;
}
