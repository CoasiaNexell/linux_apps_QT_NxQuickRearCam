//------------------------------------------------------------------------------
//
//	Copyright (C) 2016 Nexell Co. All Rights Reserved
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include "NX_CV4l2Camera.h"
#include "NX_CDrmDisplay.h"

#include <nx-v4l2.h>
#include <nx_video_api.h>
#include <linux/videodev2.h>
#include <videodev2_nxp_media.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_fourcc.h>
#include <nexell/nexell_drm.h>

#include <NX_CGpioControl.h>


static void signal_handler( int32_t sig );
static void register_signal( void );

//
//	Application Definition
//
#define DISPLAY_WIDTH		1024
#define DISPLAY_HEIGHT		600

#define	CAM_WIDTH			704
#define	CAM_HEIGHT			480
#define	USE_HDMI			1

#define	DSP_PLANE_ID		18
#define DSP_CRTC_ID			23
// #define	DSP_PLANE_ID		21
// #define DSP_CRTC_ID			26


//	Memory Information
#define	MEM_NUM_PLANE		1

#define	NUM_VID_MEM			8

int32_t main( void )
{
	//	Camera
	NX_CV4l2Camera *pV4l2Camera = NULL;
	NX_VIP_INFO camInfo;
	NX_VID_MEMORY_INFO *camBuffer[NUM_VID_MEM] = {0,};

	//	For Display
	NX_DISPLAY_HANDLE hDsp = NULL;
	NX_DISPLAY_INFO dspInfo;
	NX_CDrmDisplay *pDisplay = NULL;

	//	Register signal handler
	register_signal();

	//	Camera information
	memset( &camInfo, 0x00, sizeof(NX_VIP_INFO) );

	camInfo.iModule		= 1;
	camInfo.iSensorId	= nx_sensor_subdev;
	camInfo.iWidth		= CAM_WIDTH;
	camInfo.iHeight		= CAM_HEIGHT;
	camInfo.iFpsNum		= 30;
	camInfo.iFpsDen		= 1;
	camInfo.iNumPlane	= MEM_NUM_PLANE;
	camInfo.iCropX		= 0;
	camInfo.iCropY		= 0;
	camInfo.iCropWidth	= CAM_WIDTH;
	camInfo.iCropHeight	= CAM_HEIGHT;
	camInfo.iOutWidth	= CAM_WIDTH;
	camInfo.iOutHeight	= CAM_HEIGHT;


	//
	//	Allocate Video Memory
	//
	for( int32_t i = 0 ; i<NUM_VID_MEM ; i++ )
	{
		camBuffer[i] = NX_AllocateVideoMemory( camInfo.iWidth, camInfo.iHeight,
										MEM_NUM_PLANE, DRM_FORMAT_YUV420, 512 );
		//NX_MapVideoMemory(camBuffer[i]);
	}

	//
	//	Initialize Camera
	//
	pV4l2Camera = new NX_CV4l2Camera();
	for( int32_t i = 0 ; i<NUM_VID_MEM ; i++ )
	{
		pV4l2Camera->AddVideoMemory(camBuffer[i]);
	}
	if( 0 > pV4l2Camera->Init( &camInfo ) )
	{
		delete pV4l2Camera;
		pV4l2Camera = NULL;
		return -1;
	}

	//
	//	Initialize Display
	//
	//	Display information
	memset( &dspInfo, 0x00, sizeof(dspInfo) );
	dspInfo.planeId				= DSP_PLANE_ID;
	dspInfo.ctrlId				= DSP_CRTC_ID;
	dspInfo.width				= camInfo.iWidth;
	dspInfo.height				= camInfo.iHeight;
	dspInfo.stride				= 0;
	dspInfo.drmFormat			= DRM_FORMAT_YUV420;
	dspInfo.numPlane			= 1;//MEM_NUM_PLANE;
	dspInfo.dspSrcRect.left		= 0;
	dspInfo.dspSrcRect.top		= 0;
	dspInfo.dspSrcRect.right	= camInfo.iWidth;
	dspInfo.dspSrcRect.bottom	= camInfo.iHeight;
	dspInfo.dspDstRect.left		= 0;
	dspInfo.dspDstRect.top		= 0;
	dspInfo.dspDstRect.right	= DISPLAY_WIDTH;
	dspInfo.dspDstRect.bottom	= DISPLAY_HEIGHT;


	drmDropMaster( camBuffer[0]->drmFd );

	pDisplay = new NX_CDrmDisplay();
	pDisplay->DspInit( &dspInfo );

	int32_t bufIdx = -1;
	NX_VID_MEMORY_INFO *pImgBuf = NULL;
	while (1)
	{
		pV4l2Camera->DequeueBuffer( &bufIdx );

		pDisplay->QueueBuffer( bufIdx, camBuffer[bufIdx] );
		pV4l2Camera->QueueBuffer( bufIdx  );
	}


	pDisplay->DspClose();
	delete pDisplay;

	if( pV4l2Camera )
	{
		pV4l2Camera->Deinit();
		delete pV4l2Camera;
		pV4l2Camera = NULL;
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////////////
//																			//
//								Signal Handler								//
//																			//
//////////////////////////////////////////////////////////////////////////////
static void signal_handler( int32_t sig )
{
	printf("Aborted by signal %s (%d)..\n", (char*)strsignal(sig), sig);

	switch( sig )
	{
		case SIGINT :
			printf("SIGINT..\n"); 	break;
		case SIGTERM :
			printf("SIGTERM..\n");	break;
		case SIGABRT :
			printf("SIGABRT..\n");	break;
		default :
			break;
	}
	exit(EXIT_FAILURE);
}

static void register_signal( void )
{
	signal( SIGINT,  signal_handler );
	signal( SIGTERM, signal_handler );
	signal( SIGABRT, signal_handler );
}
//////////////////////////////////////////////////////////////////////////////
