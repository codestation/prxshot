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

#include <string.h>
#include <malloc.h>
#include <pspctrl.h>
#include "libcpp.h"
#include "Settings.hpp"
#include "minIni.h"
#include "logger.h"

#define SETTINGS_FILENAME "prxshot.ini"
#define BMP_SIZE 391734
#define ALIGN(x, y) (((x) + ((y)-1)) & ~((y)-1))

Settings::Settings(const char *path) {
    filename = strjoin(path, SETTINGS_FILENAME);
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
    free(filename);
}
