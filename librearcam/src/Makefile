########################################################################
# Get Linux Build Enviornment:
include ../build.env

CFLAGS		+= -fPIC -mfloat-abi=hard
CPPFLAGS	+= -fPIC -mfloat-abi=hard

LIBPATH 	?= $(SDKTARGETSYSROOT)/usr/lib
INCPATH 	?= $(SDKTARGETSYSROOT)/usr/include

#########################################################################
#																		#
#								Includes								#
#																		#
#########################################################################
#	Local Application Includes

INCLUDE		?= -I./ -I../include -I$(INCPATH) -I$(INCPATH)/libdrm -I$(INCPATH)/drm -I$(INCPATH)/libkms -I$(INCPATH)/nexell

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
LIBRARY 	?= -L$(LIBPATH) -L$(LIBPATH)/ldrm -L$(LIBPATH)/lkms -lnx_video_api -lnx_v4l2  -ldrm -lkms

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
