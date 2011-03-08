TARGET = prxshot
OBJS = main.o bitmap.o imports.o exports.o sfo.o pbp.o payload.o hook_module.o
# logger.o

LIBS =
CFLAGS = -Os -G0 -Wall -std=c99
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

PSP_FW_VERSION=500

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
