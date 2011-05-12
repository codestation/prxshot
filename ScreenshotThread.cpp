
#include <string.h>
#include "ScreenshotThread.hpp"
#include "Settings.hpp"
#include "PspHandler.hpp"
#include "PbpBlock.hpp"
#include "SceIo.hpp"
#include "logger.h"

ScreenshotThread::ScreenshotThread(int args, void *argp) {
    this->argp = new char[args];
    strcpy(this->argp, (char *)argp);
}

char *ScreenshotThread::createScreenshotDir(const char *gameid) {
    char *path = new char[32];
    if(settings->forceMemoryStick() || psp.getModel() < PspHandler::MODEL_GO) {
        strcpy(path, "ms0:");
    } else {
        strcpy(path, "ef0:");
    }
    strcat(path,"/PSP/SCREENSHOT");
    SceIo::mkdir(path);
    strcat(path, "/");
    strcat(path, gameid);
    return path;
}

int ScreenshotThread::run() {
    settings = new Settings((const char *)argp);
    settings->loadDefaults();
    if(settings->forceMemoryStick())
        SceIo::mkdir("ms0:/PSP");
    screen = new Screenshot();
    pbp = (psp.bootFrom() == PspHandler::DISC) ? new PbpBlock() : new PbpBlock(psp.getPBPPath());
    pbp->load();
    char *shot_path = createScreenshotDir(pbp->getSFO()->getStringValue("DISC_ID"));
    SceIo::mkdir(shot_path);
    pbp->outputDir(shot_path);
    kprintf("Shot_path: %s, format: %s\n", shot_path, settings->getScreenshotFormat());
    screen->setPath(shot_path, settings->getScreenshotFormat());
    kprintf("Starting loop\n");
    while(screen->getID()) {
        if(psp.isPressed(settings->getKeyPress())) {
            kprintf("Taking screenshot\n");
            screen->takePicture();
            screen->updateFilename();
            if(!pbp->created()) {
                pbp->start("pscm_th");
            }
        }
        Thread::delay(100000);
    }
    return 0;
}

ScreenshotThread::~ScreenshotThread() {
    delete argp;
}
