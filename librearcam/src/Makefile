########################################################################
# Get Linux Build Enviornment:
include ../build.env
######################################################################
#
#	CameraTest Application
#
# Build options
SYSROOT_DIR	:= /opt/poky/2.1.1/sysroots/cortexa9hf-neon-poky-linux-gnueabi
CFLAGS		+= -fPIC -mfloat-abi=hard --sysroot=$(SYSROOT_DIR)
CPPFLAGS	+= -fPIC -mfloat-abi=hard --sysroot=$(SYSROOT_DIR)

#########################################################################
#																		#
#								Includes								#
#																		#
#########################################################################
#	Local Application Includes
INCLUDE		+= -I./ -I../include

INCLUDE		+= -I$(LIBSDIR)/include
INCLUDE		+= -I$(LIBSDIR)/../include

#	DRM/KMS
INCLUDE		+= -I$(LIBSDIR)/include/libdrm
INCLUDE		+= -I$(LIBSDIR)/include/libkms
#	V4L2 Driver
INCLUDE		+= -I$(LIBSDIR)/include/nexell
#	Other Private Includes
INCLUDE		+= -I/opt/poky/2.1.1/sysroots/cortexa9hf-neon-poky-linux-gnueabi/usr/include

#########################################################################
#																		#
#								Sources									#
#																		#
#########################################################################
COBJS		:=
# Library Source
CPPOBJS		:=	NX_CV4l2Camera.o \
				NX_CDrmDisplay.o \
				NX_CBackgearDetect.o
# Utils
CPPOBJS		+=	NX_DbgMsg.o \
				NX_CGpioControl.o
# Manager
CPPOBJS		+=	NX_CRearCamMgr.o

#########################################################################
#																		#
#								Library									#
#																		#
#########################################################################
#	DRM
LIBRARY 	+= -L$(LIBSDIR)/lib -L$(LIBSDIR)/lib/ldrm -L$(LIBSDIR)/lib/lkms
LIBRARY 	+= -lnx_video_api -lnx_v4l2  -ldrm -lkms
LIBRARY 	+= -L../../../lib
LIBRARY		+= -lstdc++ -lpthread
LIBRARY		+= --sysroot=$(SYSROOT_DIR)

#########################################################################
#																		#
#								Target									#
#																		#
#########################################################################
# Target
TARGET		:= libnxrearcam.so

######################################################################
# Build
OBJS	:= $(COBJS) $(CPPOBJS)

all: $(TARGET)

$(TARGET): depend $(OBJS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $@ $(OBJS) $(LIBRARY)
	cp -a $(TARGET) ../lib/
	@echo "done"

install:
	cp -a $(TARGET) ../lib/

clean:
	rm -f $(TARGET) *.o .depend

#########################################################################
# Dependency
ifeq (.depend,$(wildcard .depend))
include .depend
endif

SRCS := $(COBJS:.o=.c) $(CPPOBJS:.o=.cpp)
INCS := $(INCLUDE)
depend dep:
	$(CC)  -M  $(CPPFLAGS)   $(INCS) $(SRCS) > .depend
