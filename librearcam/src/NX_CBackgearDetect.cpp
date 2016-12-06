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
//	Module		: Back Gear Detect Module for Auto Mobile
//	File		: NX_CBackgearDetect.xxx
//	Description	:
//	Author		: SeongO Park (ray@nexell.co.kr)
//	Export		:
//	History		:
//
//------------------------------------------------------------------------------

// #define NX_DBG_OFF

#ifdef NX_DTAG
#undef NX_DTAG
#endif
#define NX_DTAG "[NX_CBackgearDetect]"

#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include <NX_RearCam.h>

#include "NX_CGpioControl.h"
#include "CNX_BaseClass.h"
#include "NX_DbgMsg.h"

class NX_CBackgearDetect : protected CNX_Thread
{
public:
	void RegEventCallback( void *, void (*cbFunc)(void *, int32_t) );
	void StartService( int64_t iDetectDelay );
	void StopService();

	NX_CBackgearDetect();
	~NX_CBackgearDetect();

	//  Implementation Pure Virtual Function
	virtual void  ThreadProc();

	static NX_CBackgearDetect *GetInstance();
	static NX_CBackgearDetect *m_pSingleTone;

private:
	//
	// Hardware Depend Parameter
	//
	enum { BACKGEAR_GPIO_PORT = ALIVE3 };

	static NX_CBackgearDetect* m_pBackgearDetect;

	NX_CGpioControl*	m_pGpioControl;
	int32_t				m_iCurGearStatus;

	int32_t				m_bExitLoop;

	//	for Callback Function
	void*				m_pCbAppData;
	void				(*m_EventCallBack)(void *, int32_t);


private:
	NX_CBackgearDetect (const NX_CBackgearDetect &Ref);
	NX_CBackgearDetect &operator=(const NX_CBackgearDetect &Ref);
};


//------------------------------------------------------------------------------
NX_CBackgearDetect::NX_CBackgearDetect()
	: m_pGpioControl ( NULL )
	, m_bExitLoop ( false )
	, m_pCbAppData (NULL)

{
	m_EventCallBack = NULL;
	m_pGpioControl = new NX_CGpioControl();
	m_pGpioControl->Init( BACKGEAR_GPIO_PORT );
	m_pGpioControl->SetDirection( GPIO_DIRECTION_IN );
	m_pGpioControl->SetEdge( GPIO_EDGE_BOTH );
}

//------------------------------------------------------------------------------
NX_CBackgearDetect::~NX_CBackgearDetect()
{
	m_pGpioControl->ResetInterrupt();
	pthread_join( m_hThread, NULL );

	if( m_pGpioControl )
	{
		m_pGpioControl->Deinit();
		delete m_pGpioControl;
	}
}

void NX_CBackgearDetect::RegEventCallback( void *pAppData, void (*cbFunc)(void *, int32_t) )
{
	m_pCbAppData = pAppData;
	m_EventCallBack = cbFunc;
}

void NX_CBackgearDetect::StartService( int64_t iDetectDelay )
{
	m_bExitLoop = false;
	Start();
}

void NX_CBackgearDetect::StopService()
{
	m_bExitLoop = true;
	Stop();
}

void  NX_CBackgearDetect::ThreadProc()
{
	int32_t readStatus;
	m_iCurGearStatus = -1;

	do{
		readStatus = m_pGpioControl->GetValue();
		if( m_iCurGearStatus != readStatus )
		{
			if(m_EventCallBack)
			{
				m_EventCallBack(m_pCbAppData, readStatus );
			}
		}
		m_iCurGearStatus = readStatus;
		usleep( 10000 );

	}while( !m_bExitLoop );

}



//
//
//	Make Singletone Instance
//
//
NX_CBackgearDetect *NX_CBackgearDetect::m_pSingleTone = NULL;

NX_CBackgearDetect *NX_CBackgearDetect::GetInstance()
{
	if( NULL == m_pSingleTone )
	{
		m_pSingleTone = new NX_CBackgearDetect();
	}
	return m_pSingleTone;
}



//
//		External Interface
//
int32_t StartBackGearDetectService()
{
	NX_CBackgearDetect *pInst = (NX_CBackgearDetect *)NX_CBackgearDetect::GetInstance();
	pInst->StartService(0);
	return 0;
}

void StopBackGearDectectService()
{
	NX_CBackgearDetect *pInst = (NX_CBackgearDetect *)NX_CBackgearDetect::GetInstance();
	pInst->StopService();
}

void RegisterBackGearEventCallBack(void *pAppData, void (*callback)(void *, int32_t))
{
	NX_CBackgearDetect *pInst = (NX_CBackgearDetect *)NX_CBackgearDetect::GetInstance();
	pInst->RegEventCallback( pAppData, callback );
}
