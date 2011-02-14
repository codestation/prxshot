/*
 *  MHP3patch user module
 *
 *  Copyright (C) 2010  Codestation
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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string.h>

#define LOGFILE "ms0:/screenshot.log"

extern char buffer_log[256];

int sprintf(char *str, const char *format, ...);

#define log(format, ...) sprintf(buffer_log, format, ## __VA_ARGS__); \
                          logger(buffer_log)

int logger(const char * string);
int appendLog(const char * path, void * buffer, int buflen);

#endif /* LOGGER_H_ */
