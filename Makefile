TARGET = SimpleBatteryViewer
BUILD_PRX = 1

OBJS =	main.o

PRX_EXPORTS = exports.exp
USE_KERNEL_LIBC	= 1
USE_KERNEL_LIBS	= 1

INCDIR = $(CLASSG_LIBS)
CFLAGS = -Os -G0 -Wall -fno-strict-aliasing -fno-builtin-printf -DLNGJPN
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR			= 
LDFLAGS		=	-mno-crt0 -nostartfiles
LIBS = -lpsppower libcmlibMenu.a



PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

all:

