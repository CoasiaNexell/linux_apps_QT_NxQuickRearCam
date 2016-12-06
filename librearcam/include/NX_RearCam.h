#ifndef __NX_REARCAM_H__
#define __NX_REARCAM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


enum{
	DSP_MODULE_DRM,
	DSP_MODULE_QT,
};

enum {
	CB_TYPE_BUFFER,
	CB_TYPE_CMD_HIDE,
	CB_TYPE_CMD_SHOW,
};

typedef struct tagCAMERA_INFO{
    int32_t     iModule;
    int32_t     iSensorId;

    int32_t     iWidth;
    int32_t     iHeight;

	int32_t		iCropX;
	int32_t		iCropY;
    int32_t     iCropWidth;
    int32_t     iCropHeight;
    int32_t     iOutWidth;
    int32_t     iOutHeight;
}CAMERA_INFO;

typedef struct tagDISPLAY_INFO{
    int32_t     iPlaneId;       //  DRM Plane ID
    int32_t     iCrtcId;        //  DRM CRTC ID
	uint32_t	uDrmFormat;		//	DRM Data Format
    int32_t     iSrcWidth;      //  Input Image's Width
    int32_t     iSrcHeight;     //  Input Image's Height
    int32_t     iCropX;         //  Input Source Position
    int32_t     iCropY;
    int32_t     iCropWidth;
    int32_t     iCropHeight;
    int32_t     iDspX;          //  Display Position
    int32_t     iDspY;
    int32_t     iDspWidth;
    int32_t     iDspHeight;
}DISPLAY_INFO;

int32_t StartRearCamService(CAMERA_INFO *, DISPLAY_INFO *);
void StopRearCamService();
int32_t SetRearCamPosition( int32_t x, int32_t y, int32_t width, int32_t height );

int32_t ShowPreview();
int32_t HidePreview();

//
// Callback Function Format
//  : int32_t callback( void *pAppp, int32_t type, void *data, int32_t datasize );
//
//	Callback for Rendering
//	if callback is not set, use drm video layer.
void RegQTRenderCallback( void *pApp, int32_t (callback)(void *, int32_t, void *, int32_t) );
//	Callback for Control
void RegControlCallback( void *pApp, int32_t (callback)(void *, int32_t, void *, int32_t) );


//
//
//	Back Gear Detect
//
//
enum {
	BACKGEAR_ON = 0,
	BACKGEAR_OFF = 1
};

int32_t StartBackGearDetectService();
void StopBackGearDectectService();
void RegisterBackGearEventCallBack(void *, void (*callback)(void *, int32_t));


#ifdef __cplusplus
}
#endif

#endif
