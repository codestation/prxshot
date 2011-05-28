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

#ifndef SETTINGS_H_
#define SETTINGS_H_

class Settings {
public:
    Settings(const char *path);
    ~Settings();
    void loadDefaults();
    void loadCustomKey(const char *gameid);
    int getKeyPress() { return key_button; }
    const char *getScreenshotFormat() { return screenshot_format; };
    int forceMemoryStick() { return force_ms0; }
    int clearCache() { return clear_cache; }
private:
    char *filename;
    int key_button;
    char screenshot_format[32];
    int force_ms0;
    int clear_cache;
    int create_pic1;
};

#endif /* SETTINGS_H_ */
