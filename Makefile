TARGET = prxshot
OBJS = main.o bitmap.o imports.o exports.o sfo.o pbp.o payload.o hook_module.o kalloc.o minIni.o
# logger.o
LIBS =
MININI_DEFINES = -DNDEBUG -DINI_READONLY -DINI_FILETYPE=SceUID -DPORTABLE_STRNICMP -DINI_NOFLOAT
CFLAGS = -Os -G0 -Wall -std=c99 $(MININI_DEFINES)
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

PSP_FW_VERSION=500

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
