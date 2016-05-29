#pragma once

#include <NuiKinectFusionApi.h>
#include <string>
#include "Timer.h"
#include <vector>

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if (p) { delete (p); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#endif

#ifndef SAFE_FUSION_RELEASE_IMAGE_FRAME
#define SAFE_FUSION_RELEASE_IMAGE_FRAME(p) { if (p) { static_cast<void>(NuiFusionReleaseImageFrame(p)); (p)=NULL; } }
#endif

struct KinectScanData
{
    unsigned int width, height, length;
    std::vector<bool> data;
};

class KinectScannerFusion
{
    static const int bytesPerPixel = 4;
    static const int resetOnGapSize = 10000;
    static const int resetOnLostFrameCount = 1000;
	const std::string reconstructionWindow = "reconstructionWindow";

public:

	KinectScannerFusion();
    ~KinectScannerFusion();

    void Run();
    KinectScanData GetScanData() const;

private:
   
    INuiSensor* kinectSensor;
    int width;
    int height;
    int frameSize;
    HANDLE depthStream;
    HANDLE nextFrameReadyEvent;
    LARGE_INTEGER lastFrameTimeStamp;
	INuiFusionReconstruction*   voxelVolume;
	NUI_FUSION_RECONSTRUCTION_PARAMETERS reconstructionParams;
	Matrix4 worldToCameraTransform;
	Matrix4 defaultWorldToVolumeTransform;
	NUI_DEPTH_IMAGE_PIXEL* depthImagePixelBuffer;
	NUI_FUSION_IMAGE_FRAME* depthFloatFrame;
	NUI_FUSION_IMAGE_FRAME* pointCloudFrame;
	NUI_FUSION_IMAGE_FRAME* raycastedFrame;
	int lostFrameCounter;
	bool trackingFailed;
	int frameCounter;
	double startTime;
	Timer timer;

    void Update();
    HRESULT InitKinectSensor();
    HRESULT InitVoxelVolume();
    HRESULT ExtractFrameData(NUI_IMAGE_FRAME &imageFrame);
    void ProcessFrame();
    HRESULT ResetVolume();
	void SetErrorMessage(WCHAR * szMessage);
};
