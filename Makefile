TARGET = prxshot
CLASS_OBJS =    PbpBlock.o \
				Settings.o \
				SfoBlock.o \
				Screenshot.o \
				Thread.o \
				ScreenshotThread.o \
				PspHandler.o \
				GlobalBuffer.o
OBJS = main.o bitmap.o imports.o exports.o minIni.o logger.o  kmalloc.o libcpp.o $(CLASS_OBJS)
LIBS =
MININI_DEFINES = -DNDEBUG -DINI_READONLY -DINI_FILETYPE=SceUID -DPORTABLE_STRNICMP -DINI_NOFLOAT
SHARED_FLAGS = -O2 -G0 -Wall
# -DDEBUG_MEMORY
CFLAGS = $(SHARED_FLAGS) $(MININI_DEFINES)
# -DKPRINTF_ENABLED
CXXFLAGS = $(SHARED_FLAGS) -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

PSP_FW_VERSION=500

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
