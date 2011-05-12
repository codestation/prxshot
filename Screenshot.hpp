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
