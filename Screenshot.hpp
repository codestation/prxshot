/*
 * Screenshot.h
 *
 *  Created on: 06/05/2011
 *      Author: code
 */

#ifndef SCREENSHOT_H_
#define SCREENSHOT_H_

class Screenshot {
    int last_id;
    int max_id;
    char *path;
    char *format;
    char filename[256];
public:
    Screenshot(int s_max_id = 10000) : last_id(0),
                                       max_id(s_max_id),
                                       path(NULL),
                                       format(NULL) {}
    void setPath(const char *s_path, const char *s_format);
    bool takePicture();
    inline int getID() { return last_id; };
    int updateFilename();
    const char *getFilename();
    ~Screenshot();
};

#endif /* SCREENSHOT_H_ */
