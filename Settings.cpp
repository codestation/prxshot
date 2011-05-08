/*
 * Settings.cpp
 *
 *  Created on: 06/05/2011
 *      Author: code
 */

#include <string.h>
#include <pspctrl.h>
#include "Settings.hpp"
#include "minIni.h"

Settings::Settings(const char *path) {
    filename = new char[64];
    strcpy(filename, path);
    strrchr(filename, '/')[1] = 0;
    strcpy(filename + strlen(filename), "prxshot.ini");
}

void Settings::loadDefaults() {
    key_button = ini_getlhex("General", "ScreenshotKey", PSP_CTRL_NOTE, filename);
    ini_gets("General", "ScreenshotName", "%s/pic_%04d.bmp", screenshot_format, sizeof(screenshot_format), filename);
    force_ms0 = ini_getbool("General", "PSPGoUseMS0", 0, filename);
    clear_cache = ini_getbool("General", "XMBClearCache", 0, filename);
}

void Settings::loadCustomKey(const char *gameid) {
    key_button = ini_getlhex("CustomKeys", gameid, key_button, filename);
}

Settings::~Settings() {
    delete[] filename;
}
