TARGET = screenshot
OBJS = main.o bitmap.o imports.o sfo.o pbp.o logger.o
PSP_FW_VERSION=550
BUILD_PRX=1

PRX_EXPORTS=exports.exp

INCDIR =
CFLAGS = -O2 -G0 -Wall -std=c99
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR =
LIBS =

LDFLAGS = -mno-crt0 -nostartfiles
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
