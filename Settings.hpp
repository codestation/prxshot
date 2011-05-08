/*
 * Settings.h
 *
 *  Created on: 06/05/2011
 *      Author: code
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define BMP_SIZE 391734
#define ALIGN(x, y) (((x) + ((y)-1)) & ~((y)-1))

class Settings {
    char *filename;
    int key_button;
    char screenshot_format[32];
    int force_ms0;
    int clear_cache;
public:
    Settings(const char *path);
    void loadDefaults();
    void loadCustomKey(const char *gameid);
    int getKeyPress() { return key_button; }
    const char *getScreenshotFormat() { return screenshot_format; };
    int forceMemoryStick() { return force_ms0; }
    int clearCache() { return clear_cache; }
    ~Settings();
};

#endif /* SETTINGS_H_ */
