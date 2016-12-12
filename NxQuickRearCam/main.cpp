#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mainwindow.h"
#include <QApplication>
#include <QObject>

#include <unistd.h>	//	usleep
#include <nx-v4l2.h>
#include <NX_RearCam.h>
#include <drm/drm_fourcc.h>

#include "eventsender.h"

static bool m_bInitialized = false;
static CAMERA_INFO m_CamInfo;
static DISPLAY_INFO m_DspInfo;

#define USE_SIGNAL_HANDLING		(1)

bool StartRearCam( int32_t planeId, int32_t crtcId );
void StopRearCam();

bool StartRearCam( int32_t planeId, int32_t crtcId, QRect rect )
{
	int32_t camWidth = 704;
	int32_t camHeight = 480;

	memset( &m_CamInfo, 0, sizeof(m_CamInfo) );
	memset( &m_DspInfo, 0, sizeof(m_DspInfo) );

	//  Camera Information
	m_CamInfo.iModule		= 1;
	m_CamInfo.iSensorId     = nx_sensor_subdev;
	m_CamInfo.iWidth		= camWidth;
	m_CamInfo.iHeight		= camHeight;
	m_CamInfo.iCropX		= 0;
	m_CamInfo.iCropY		= 0;
	m_CamInfo.iCropWidth	= camWidth;
	m_CamInfo.iCropHeight	= camHeight;
	m_CamInfo.iOutWidth     = camWidth;
	m_CamInfo.iOutHeight	= camHeight;


	//	Get graphic view's rect
	m_DspInfo.iPlaneId      = planeId;
	m_DspInfo.iCrtcId		= crtcId;
	m_DspInfo.uDrmFormat	= DRM_FORMAT_YUV420;
	m_DspInfo.iSrcWidth     = camWidth;
	m_DspInfo.iSrcHeight	= camHeight;
	m_DspInfo.iCropX		= 0;
	m_DspInfo.iCropY		= 0;
	m_DspInfo.iCropWidth	= camWidth;
	m_DspInfo.iCropHeight	= camHeight;
	m_DspInfo.iDspX         = rect.x();
	m_DspInfo.iDspY         = rect.y();
	m_DspInfo.iDspWidth 	= rect.width();
	m_DspInfo.iDspHeight	= rect.height();

	if( 0 != StartRearCamService( &m_CamInfo, &m_DspInfo ) )
	{
		m_bInitialized = false;
		return false;
	}
	m_bInitialized = true;

	return true;
}

void StopRearCam()
{
	if( m_bInitialized )
	{
		StopRearCamService();
		m_bInitialized = false;
	}
}

static void BackGearCallback( void *pApp, int32_t nOnOff )
{
	EventSender *pObj = (EventSender *)pApp;

	if( nOnOff )
	{
		//	Off
		HidePreview();	//	Preview Off
		pObj->HideSignal(true);
	}
	else
	{
		//	On
		pObj->HideSignal(false);
		ShowPreview();	//	UI On
	}
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	QRect rect;

#if USE_SIGNAL_HANDLING
	EventSender eventSender;
#endif

	int32_t planeId = 27;
	int32_t crtcId = 26;

	// Stop Kernel Layer Rear Camera Service
	// This command is locked interface
	char strBuf[64];
	memset(strBuf, 0x00, 64);

	int32_t fd = open( "/sys/devices/platform/nx-rearcam/stop", O_RDONLY );
	read( fd, strBuf, sizeof(strBuf) );
	if( !strncmp( strBuf, "1", sizeof(strBuf) ) )
	{
		close(fd );
		system("echo 1 > /sys/devices/platform/nx-rearcam/stop");
	}
	else
	{
		close(fd );
	}

	w.setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
	w.setGeometry(0,0,1024,600);
	w.GetGraphicViewRect(rect);

	QObject::connect( &eventSender, SIGNAL(setHideEvent(bool)), &w, SLOT(setHideEvent(bool)) );
	RegisterBackGearEventCallBack( &eventSender, BackGearCallback );

	StartBackGearDetectService();

	StartRearCam( planeId, crtcId, rect );

	return a.exec();
}
