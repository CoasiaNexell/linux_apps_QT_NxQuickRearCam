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

#include <string.h>
//#include <nx_fourcc.h>

#include "CNX_BaseClass.h"
#include "NX_CDrmDisplay.h"

//#define NX_DBG_OFF

////////////////////////////////////////////////////////////////////////////////
//
//	NX_CDrmDisplay
//
#ifdef NX_DTAG
#undef NX_DTAG
#endif
#define NX_DTAG		"[NX_CDrmDisplay]"
#include "NX_DbgMsg.h"

#define ROUND_UP_16(num) (((num)+15)&~15)
#define ROUND_UP_32(num) (((num)+31)&~31)

//------------------------------------------------------------------------------
NX_CDrmDisplay::NX_CDrmDisplay()
{
	int32_t i = 0;
	for( i = 0 ; i < MAX_INPUT_BUFFER; i++ )
	{
		m_BufferId[i] = 0;
	}
	m_LastBuferIndex = -1;

	pthread_mutex_init( &m_hDspCtrlLock, NULL );
}

//------------------------------------------------------------------------------
NX_CDrmDisplay::~NX_CDrmDisplay()
{
	pthread_mutex_destroy( &m_hDspCtrlLock );
}

//------------------------------------------------------------------------------
int32_t NX_CDrmDisplay::DrmIoctl( int32_t fd, unsigned long request, void *pArg )
{
	int32_t ret;

	do {
		ret = ioctl(fd, request, pArg);
	} while (ret == -1 && (errno == EINTR || errno == EAGAIN));

	return ret;
}

//------------------------------------------------------------------------------
int32_t NX_CDrmDisplay::ImportGemFromFlink( int32_t fd, uint32_t flinkName )
{
	struct drm_gem_open arg;

	memset( &arg, 0x00, sizeof( drm_gem_open ) );

	arg.name = flinkName;
	if (DrmIoctl(fd, DRM_IOCTL_GEM_OPEN, &arg)) {
		return -EINVAL;
	}

	return arg.handle;
}

//------------------------------------------------------------------------------
int32_t NX_CDrmDisplay::DspInit( NX_DISPLAY_INFO *pDspInfo )
{
	int32_t hDrmFd = -1;
	int32_t i = 0;

	for( i = 0 ; i < MAX_INPUT_BUFFER; i++ )
	{
		m_BufferId[i] = 0;
	}

	//
	// FIX ME!! ( DO NOT MOVE THIS CODE )
	//   It is seems to be bugs.
	//   If this drmOpen() is called late than another drmOpen(), drmModeSetPlane() is failed.
	//
	hDrmFd = drmOpen( "nexell", NULL );
	if( 0 > hDrmFd )
	{
		NxDbgMsg( NX_DBG_ERR, "Fail, drmOpen().\n" );
		return -1;
	}

	drmSetMaster( hDrmFd );

	if( 0 > drmSetClientCap(hDrmFd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1) )
	{
		drmClose( hDrmFd );
		NxDbgMsg( NX_DBG_ERR, "Fail, drmSetClientCap().\n" );
		return -1;
	}

	m_DspInfo = *pDspInfo;
	m_DspInfo.drmFd = hDrmFd;
	return 0;
}

//------------------------------------------------------------------------------
void NX_CDrmDisplay::DspClose()
{
	int32_t i = 0;

	if( m_DspInfo.drmFd > 0 )
	{
		// clean up object here
		for( i = 0; i < MAX_INPUT_BUFFER; i++ )
		{
			if( 0 < m_BufferId[i] )
			{
				drmModeRmFB( m_DspInfo.drmFd, m_BufferId[i] );
				m_BufferId[i] = 0;
			}
		}

		if( 0 <= m_DspInfo.drmFd )
		{
			drmClose( m_DspInfo.drmFd );
			m_DspInfo.drmFd = -1;
		}
		memset( &m_DspInfo, 0, sizeof(m_DspInfo) );
		m_DspInfo.drmFd = -1;
	}
}

//------------------------------------------------------------------------------
int32_t NX_CDrmDisplay::QueueBuffer( int32_t bufferIdx, NX_VID_MEMORY_INFO *pImg )
{
	int32_t err = 0;

	if( m_BufferId[bufferIdx] == 0 )
	{
		int32_t i = 0;
		uint32_t handles[4] = { 0, };
		uint32_t pitches[4] = { 0, };
		uint32_t offsets[4] = { 0, };
		uint32_t offset = 0;
		uint32_t strideWidth[3] = { 0, };
		uint32_t strideHeight[3] = { 0, };

		if (1 == pImg->planes)
		{
			strideWidth[0] = ROUND_UP_32(pImg->stride[0]);
			strideWidth[1] = ROUND_UP_16(strideWidth[0] >> 1);
			strideWidth[2] = strideWidth[1];
			strideHeight[0] = ROUND_UP_16(pImg->height);
			strideHeight[1] = ROUND_UP_16(pImg->height >> 1);
			strideHeight[2] = strideHeight[1];
		}
		else
		{
			strideWidth[0] = pImg->stride[0];
			strideWidth[1] = pImg->stride[1];
			strideWidth[2] = pImg->stride[2];
			strideHeight[0] = ROUND_UP_16(pImg->height);
			strideHeight[1] = ROUND_UP_16(pImg->height >> 1);
			strideHeight[2] = strideHeight[1];
		}

		//	FIXME: Hardcoded "3"
		for( i = 0; i <3 /*pImg->planes*/; i++ )
		{
			handles[i] = (pImg->planes == 1) ?
				ImportGemFromFlink( m_DspInfo.drmFd, pImg->flink[0] ) :
				ImportGemFromFlink( m_DspInfo.drmFd, pImg->flink[i] );
			pitches[i] = strideWidth[i];
			offsets[i] = offset;
			offset += ( (pImg->planes == 1) ? (strideWidth[i] * strideHeight[i]) : 0 );
		}

		err = drmModeAddFB2( m_DspInfo.drmFd, pImg->width, pImg->height,
				m_DspInfo.drmFormat, handles, pitches, offsets, &m_BufferId[bufferIdx], 0 );
		if( 0 > err )
		{
			NxDbgMsg( NX_DBG_ERR, "Fail, drmModeAddFB2() !!!. err = %d\n", err );
			return -1;
		}

		NxDbgMsg( NX_DBG_ERR, "Resol(%dx%d), pitch(%d,%d,%d,%d)\n",
			pImg->width, pImg->height, pitches[0], pitches[1], pitches[2], pitches[3]);

	}

	{
		CNX_AutoLock lock(&m_hDspCtrlLock);

		err = drmModeSetPlane( m_DspInfo.drmFd, m_DspInfo.planeId, m_DspInfo.ctrlId, m_BufferId[bufferIdx], 0,
				m_DspInfo.dspDstRect.left, m_DspInfo.dspDstRect.top, m_DspInfo.dspDstRect.right, m_DspInfo.dspDstRect.bottom,
				m_DspInfo.dspSrcRect.left << 16, m_DspInfo.dspSrcRect.top << 16, m_DspInfo.dspSrcRect.right << 16, m_DspInfo.dspSrcRect.bottom << 16 );

		m_LastBuferIndex = bufferIdx;
	}

	if( 0 > err )
	{
		printf("Fail, drmModeSetPlane() !!!.\n");
		return -1;
	}

	return 0;
}

int32_t NX_CDrmDisplay::SetPosition( int32_t x, int32_t y, int32_t width, int32_t height )
{
	CNX_AutoLock lock(&m_hDspCtrlLock);
	m_DspInfo.dspDstRect.left = x;
	m_DspInfo.dspDstRect.top = y;
	m_DspInfo.dspDstRect.right = x + width;
	m_DspInfo.dspDstRect.bottom = y + height;

	if( m_BufferId[m_LastBuferIndex]!=0 && m_LastBuferIndex >= 0 )
	{
		int32_t err = drmModeSetPlane( m_DspInfo.drmFd, m_DspInfo.planeId, m_DspInfo.ctrlId, m_BufferId[m_LastBuferIndex], 0,
				m_DspInfo.dspDstRect.left, m_DspInfo.dspDstRect.top, m_DspInfo.dspDstRect.right, m_DspInfo.dspDstRect.bottom,
				m_DspInfo.dspSrcRect.left << 16, m_DspInfo.dspSrcRect.top << 16, m_DspInfo.dspSrcRect.right << 16, m_DspInfo.dspSrcRect.bottom << 16 );
		return err;
	}
	return 1;
}
