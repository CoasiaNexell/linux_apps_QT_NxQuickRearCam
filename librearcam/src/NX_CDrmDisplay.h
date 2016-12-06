//------------------------------------------------------------------------------
//
//	Copyright (C) 2015 Nexell Co. All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//  FOR A PARTICULAR PURPOSE.
//
//	Module		:
//	File		:
//	Description	:
//	Author		:
//	Export		:
//	History		:
//
//------------------------------------------------------------------------------

#ifndef __NX_CDRMDISPLAY_H__
#define __NX_CDRMDISPLAY_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <nx_video_api.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_fourcc.h>
#include <nexell/nexell_drm.h>
#include <errno.h>

typedef struct DSP_IMG_RECT {
	int32_t		left;
	int32_t		top;
	int32_t		right;
	int32_t		bottom;
} DSP_IMG_RECT;

typedef struct NX_DISPLAY_INFO {
	uint32_t		planeId;
	uint32_t		ctrlId;

	int32_t			width;		// source width
	int32_t			height;		// source height
	int32_t			stride;		// source image's strid
	int32_t			drmFormat;	// source image's format
	int32_t			numPlane;	// source image's plane number
	int32_t			drmFd;		//	Drm Device Handle

	DSP_IMG_RECT 	dspSrcRect;	// source image's crop region
	DSP_IMG_RECT	dspDstRect;	// target display rect
} NX_DISPLAY_INFO;

typedef struct NX_DISPLAY_INFO	*NX_DISPLAY_HANDLE;

#define MAX_INPUT_BUFFER		32
#define MAX_ALLOC_BUFFER		8
#define MAX_PLANE_NUM			4

typedef struct
{
	int32_t		width;			//	Video Image's Width
	int32_t		height;			//	Video Image's Height
	int32_t		align;			//	Start address align
	int32_t		planes;			//	Number of valid planes
	int32_t		pixelByte;		//	Pixel Bytes
	uint32_t	format;			//	Pixel Format

	int32_t		drmFd;					//	Drm Device Handle
	int32_t		dmaFd[MAX_PLANE_NUM];	//	DMA memory Handle
	int32_t		gemFd[MAX_PLANE_NUM];	//	GEM Handle
	int32_t		size[MAX_PLANE_NUM];	//	Each plane's size.
	int32_t		stride[MAX_PLANE_NUM];	//	Each plane's stride.
	void*		buffer[MAX_PLANE_NUM];	//	virtual address.
} NX_VID_MEMORY;


//------------------------------------------------------------------------------
class NX_CDrmDisplay
{
public:
	NX_CDrmDisplay();
	~NX_CDrmDisplay();

public:
	int32_t		DspInit( NX_DISPLAY_INFO *pDspInfo );
	void		DspClose();
	int32_t 	QueueBuffer( int32_t bufferIdx, NX_VID_MEMORY_INFO *pImg );
	int32_t		SetPosition( int32_t x, int32_t y, int32_t width, int32_t height );

private:
	int32_t		DrmIoctl( int32_t fd, unsigned long request, void *pArg );
	int32_t		ImportGemFromFlink( int32_t fd, uint32_t flinkName );

private:
	uint32_t		m_BufferId[MAX_INPUT_BUFFER];
	NX_DISPLAY_INFO	m_DspInfo;

	//	Last Image's Information
	int32_t			m_LastBuferIndex;

	pthread_mutex_t	m_hDspCtrlLock;

private:
	NX_CDrmDisplay (const NX_CDrmDisplay &Ref);
	NX_CDrmDisplay &operator=(const NX_CDrmDisplay &Ref);
};

#endif	// __NX_CDrmDISPLAY_H__
