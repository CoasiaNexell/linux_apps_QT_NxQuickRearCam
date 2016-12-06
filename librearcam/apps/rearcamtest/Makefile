########################################################################
# Get Linux Build Enviornment:

TOPDIR := ../..
include $(TOPDIR)/build.env
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
INCLUDE		+= -I./ -I$(TOPDIR)/include -I$(TOPDIR)/src

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
CPPOBJS		:=	NX_CameraTest.o

#########################################################################
#																		#
#								Library									#
#																		#
#########################################################################
#	DRM
LIBRARY 	+= -L$(LIBSDIR)/lib -L$(LIBSDIR)/lib/ldrm -L$(LIBSDIR)/lib/lkms
LIBRARY 	+= -lnx_video_api -lnx_drm_allocator -lnx_v4l2  -ldrm -lkms
LIBRARY 	+= -L../../../lib
LIBRARY		+= -lstdc++ -lpthread
LIBRARY		+= -L$(TOPDIR)/lib -lnxrearcam

#########################################################################
#																		#
#								Target									#
#																		#
#########################################################################
# Target
TARGET		:= rearcamtest

######################################################################
# Build
OBJS	:= $(COBJS) $(CPPOBJS)

all: $(TARGET)

$(TARGET): depend $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(TARGET) $(LIBRARY)
	cp -a $(TARGET) $(TOPDIR)/bin/
	@echo "done"

install:
	cp -a $(TARGET) $(TOPDIR)/bin/

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
