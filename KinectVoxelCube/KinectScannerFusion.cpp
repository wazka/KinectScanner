
#include <string>
#include <strsafe.h>
#include <new>

#include "KinectScannerFusion.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

void SetIdentityMatrix(Matrix4 &mat)
{
    mat.M11 = 1; mat.M12 = 0; mat.M13 = 0; mat.M14 = 0;
    mat.M21 = 0; mat.M22 = 1; mat.M23 = 0; mat.M24 = 0;
    mat.M31 = 0; mat.M32 = 0; mat.M33 = 1; mat.M34 = 0;
    mat.M41 = 0; mat.M42 = 0; mat.M43 = 0; mat.M44 = 1;
}


KinectScannerFusion::KinectScannerFusion() :
    kinectSensor(nullptr),
    depthStream(INVALID_HANDLE_VALUE),
    nextFrameReadyEvent(INVALID_HANDLE_VALUE),
    voxelVolume(nullptr),
    depthImagePixelBuffer(nullptr),
    depthFloatFrame(nullptr),
    pointCloudFrame(nullptr),
    raycastedFrame(nullptr),
    lostFrameCounter(0),
    trackingFailed(false),
    frameCounter(0),
    startTime(0)
{
    width = 640;
    height = 480;
    frameSize = width*height;

	//Rozmiar siatki i gęstość voxeli
    reconstructionParams.voxelsPerMeter = 256;
    reconstructionParams.voxelCountX = 256;
    reconstructionParams.voxelCountY = 256;
    reconstructionParams.voxelCountZ = 256;

    SetIdentityMatrix(worldToCameraTransform);
    SetIdentityMatrix(defaultWorldToVolumeTransform);

    lastFrameTimeStamp.QuadPart = 0;

	namedWindow(reconstructionWindow, cv::WINDOW_AUTOSIZE);
}

KinectScannerFusion::~KinectScannerFusion()
{
    SafeRelease(voxelVolume);

	SAFE_FUSION_RELEASE_IMAGE_FRAME(raycastedFrame);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(pointCloudFrame);
    SAFE_FUSION_RELEASE_IMAGE_FRAME(depthFloatFrame);

    if (kinectSensor)
    {
        kinectSensor->NuiShutdown();
        kinectSensor->Release();
    }

    if (nextFrameReadyEvent != INVALID_HANDLE_VALUE)
        CloseHandle(nextFrameReadyEvent);

    SAFE_DELETE_ARRAY(depthImagePixelBuffer);
}

void KinectScannerFusion::Run()
{
    if (FAILED(InitKinectSensor()))
        return;

    if (FAILED(InitVoxelVolume()))
        return;

    while (true)
    {
        Update();
        if (cv::waitKey(20) == 'q') 
            break;
    }
}

KinectScanData KinectScannerFusion::GetScanData() const
{
    KinectScanData scanData;

    scanData.width = reconstructionParams.voxelCountX / 4;
    scanData.height = reconstructionParams.voxelCountY / 4;
    scanData.length = reconstructionParams.voxelCountZ / 4;

    auto dataSize = scanData.width*scanData.height*scanData.length;

    std::vector<SHORT> voxelData(dataSize);
    voxelVolume->ExportVolumeBlock(0, 0, 0, scanData.width,
        scanData.height, scanData.length, 4, voxelData.size() * sizeof(SHORT), &voxelData[0]);

    scanData.data.resize(dataSize);
    for (auto i = 0; i < dataSize; ++i)
    {
        // !?! http://through-the-interface.typepad.com/through_the_interface/2013/10/kinect-fusion-approaches-for-point-cloud-generation-inside-autocad.html
        // nie znalazlem wytlumaczenia w dokumentacji niestety
        double v = (double)((voxelData[i] | 0xFFFF) / 0xFFFF);
        scanData.data[i] = v > 0.0;
    }

    return scanData;
}

void KinectScannerFusion::Update()
{
    if (nullptr == kinectSensor)
        return;

    if ( WAIT_OBJECT_0 == WaitForSingleObject(nextFrameReadyEvent, 0) )
        ProcessFrame();
}

HRESULT KinectScannerFusion::InitKinectSensor()
{
    INuiSensor * possiblySensor;
	HRESULT hr = S_OK;

    int sensorCount = 0;
	if (FAILED(NuiGetSensorCount(&sensorCount)))
    {
        SetErrorMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    for (int i = 0; i < sensorCount; ++i)
    {
		if (FAILED(NuiCreateSensorByIndex(i, &possiblySensor)))
            continue;

		if (S_OK == possiblySensor->NuiStatus())
        {
            kinectSensor = possiblySensor;
            break;
        }
        possiblySensor->Release();
    }

    if (nullptr != kinectSensor)
    {
		if (SUCCEEDED(kinectSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH)))
        {
            nextFrameReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
			hr = kinectSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextFrameReadyEvent, &depthStream);
        }
    }

    if (nullptr == kinectSensor || FAILED(hr))
    {
        SetErrorMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

HRESULT KinectScannerFusion::InitVoxelVolume()
{
    HRESULT hr;
	if (FAILED(hr = NuiFusionCreateReconstruction(&reconstructionParams, NUI_FUSION_RECONSTRUCTION_PROCESSOR_TYPE_AMP, -1, &worldToCameraTransform, &voxelVolume)))
    {
		SetErrorMessage(L"Failed to initialize Kinect Fusion reconstruction volume.");
		return E_FAIL;
    }

	if (FAILED(voxelVolume->GetCurrentWorldToVolumeTransform(&defaultWorldToVolumeTransform)))
    {
        SetErrorMessage(L"Failed in call to GetCurrentWorldToVolumeTransform.");
		return E_FAIL;
    }

	if (FAILED(ResetVolume()))
	{
		SetErrorMessage(L"Failed in call to ResetVolume.");
		return E_FAIL;
	}

	if (FAILED(NuiFusionCreateImageFrame(NUI_FUSION_IMAGE_TYPE_FLOAT, width, height, nullptr, &depthFloatFrame)))
    {
        SetErrorMessage(L"Failed to initialize Kinect Fusion image.");
		return E_FAIL;
    }

	if (FAILED(NuiFusionCreateImageFrame(NUI_FUSION_IMAGE_TYPE_POINT_CLOUD, width, height, nullptr, &pointCloudFrame)))
    {
        SetErrorMessage(L"Failed to initialize Kinect Fusion image.");
		return E_FAIL;
    }

	if (FAILED(NuiFusionCreateImageFrame(NUI_FUSION_IMAGE_TYPE_COLOR, width, height, nullptr, &raycastedFrame)))
    {
        SetErrorMessage(L"Failed to initialize Kinect Fusion image.");
		return E_FAIL;
    }

    depthImagePixelBuffer = new(std::nothrow) NUI_DEPTH_IMAGE_PIXEL[frameSize];
    if (nullptr == depthImagePixelBuffer)
    {
        SetErrorMessage(L"Failed to initialize Kinect Fusion depth image pixel buffer.");
        return E_FAIL;
    }

    startTime = timer.AbsoluteTime();

    return S_OK;
}

HRESULT KinectScannerFusion::ExtractFrameData(NUI_IMAGE_FRAME &imageFrame)
{
    HRESULT hr;

    if (nullptr == depthImagePixelBuffer)
    {
        SetErrorMessage(L"Error depth image pixel buffer is nullptr.");
        return E_FAIL;
    }

    INuiFrameTexture *extendedDepthTex = nullptr;
	if (FAILED(kinectSensor->NuiImageFrameGetDepthImagePixelFrameTexture(depthStream, &imageFrame, nullptr, &extendedDepthTex)))
    {
        SetErrorMessage(L"Error getting extended depth texture.");
        return E_FAIL;
    }

    NUI_LOCKED_RECT extendedDepthLockedRect;
    hr = extendedDepthTex->LockRect(0, &extendedDepthLockedRect, nullptr, 0);
    if (FAILED(hr) || extendedDepthLockedRect.Pitch == 0)
    {
        SetErrorMessage(L"Error getting extended depth texture pixels.");
        return hr;
    }

    errno_t err = memcpy_s(depthImagePixelBuffer, frameSize * sizeof(NUI_DEPTH_IMAGE_PIXEL), extendedDepthLockedRect.pBits, extendedDepthTex->BufferLen());
    extendedDepthTex->UnlockRect(0);

    if(0 != err)
        SetErrorMessage(L"Error copying extended depth texture pixels.");

    return hr;
}

void KinectScannerFusion::ProcessFrame()
{
    HRESULT hr = S_OK;
    NUI_IMAGE_FRAME imageFrame;

	if (FAILED(kinectSensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame)))
    {
        SetErrorMessage(L"Kinect NuiImageStreamGetNextFrame call failed.");
        return;
    }

	//
	cv::Mat img_depth(height, width, CV_16UC1);
	NUI_LOCKED_RECT rect2;
	if (FAILED(imageFrame.pFrameTexture->LockRect(0, &rect2, nullptr, 0)))
		return;
	if (rect2.Pitch != 0)
	{
		//assert(width * height * bytesPerPixel == rect2.size);
		memcpy(img_depth.ptr<short>(), rect2.pBits, rect2.size);
	}
	imageFrame.pFrameTexture->UnlockRect(0);
	//

    hr = ExtractFrameData(imageFrame);
    LARGE_INTEGER currentDepthFrameTime = imageFrame.liTimeStamp;
    kinectSensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);

    if (FAILED(hr))
        return;

    if (frameCounter != 0 && abs(currentDepthFrameTime.QuadPart - lastFrameTimeStamp.QuadPart) > resetOnGapSize)
    {
        ResetVolume();
        if (FAILED(hr))
            return;
    }

    lastFrameTimeStamp = currentDepthFrameTime;

    if (nullptr == voxelVolume)
    {
        SetErrorMessage(L"Kinect Fusion reconstruction volume not initialized. Please try reducing volume size or restarting.");
        return;
    }

	if (FAILED(voxelVolume->DepthToDepthFloatFrame(depthImagePixelBuffer, frameSize * sizeof(NUI_DEPTH_IMAGE_PIXEL), depthFloatFrame, NUI_FUSION_DEFAULT_MINIMUM_DEPTH, NUI_FUSION_DEFAULT_MAXIMUM_DEPTH, false)))
    {
        SetErrorMessage(L"Kinect Fusion NuiFusionDepthToDepthFloatFrame call failed.");
        return;
    }

	//
	cv::Mat img_depth_float(height, width, CV_32FC1);
	if (FAILED(depthFloatFrame->pFrameTexture->LockRect(0, &rect2, nullptr, 0)))
		return;
	if (rect2.Pitch != 0)
	{
		//assert(width * height * bytesPerPixel == rect2.size);
		memcpy(img_depth_float.ptr<short>(), rect2.pBits, rect2.size);
	}
	depthFloatFrame->pFrameTexture->UnlockRect(0);
	//

    //Tutaj jest update macierzy przekształcenia i aktualizacja modelu o info z najnowszej klatki - coś z tego pewnie trzeba przerobić na ręczne liczenie
	if (FAILED(voxelVolume->ProcessFrame(depthFloatFrame, NUI_FUSION_DEFAULT_ALIGN_ITERATION_COUNT, NUI_FUSION_DEFAULT_INTEGRATION_WEIGHT, &worldToCameraTransform)))
    {
        if (hr == E_NUI_FUSION_TRACKING_ERROR)
        {
            lostFrameCounter++;
            trackingFailed = true;
            SetErrorMessage(L"Kinect Fusion camera tracking failed! Align the camera to the last tracked position.");
        }
        else
        {
            SetErrorMessage(L"Kinect Fusion ProcessFrame call failed!");
            return;
        }
    }
    else
    {
        Matrix4 calculatedCameraPose;

		if (SUCCEEDED(voxelVolume->GetCurrentWorldToCameraTransform(&calculatedCameraPose)))
        {
            worldToCameraTransform = calculatedCameraPose;
            lostFrameCounter = 0;
            trackingFailed = false;
        }
    }

    if (trackingFailed && lostFrameCounter >= resetOnLostFrameCount)
    {
        hr = ResetVolume();
        if (FAILED(hr))
            return;

        SetErrorMessage(L"Kinect Fusion camera tracking failed, automatically reset volume.");
    }

    //Tutaj jest przekształcenie modelu i wyświetlenie go - to jest do wymiany na maszerujące czworościany
	if (FAILED(voxelVolume->CalculatePointCloud(pointCloudFrame, &worldToCameraTransform)))
    {
        SetErrorMessage(L"Kinect Fusion CalculatePointCloud call failed.");
        return;
    }

    //Raycasting
	if (FAILED(NuiFusionShadePointCloud(pointCloudFrame, &worldToCameraTransform, nullptr, raycastedFrame, nullptr)))
    {
        SetErrorMessage(L"Kinect Fusion NuiFusionShadePointCloud call failed.");
        return;
    }

	//Przekopiowanie i wyświetlenie OpenCV
    INuiFrameTexture * frameTexture = raycastedFrame->pFrameTexture;
    NUI_LOCKED_RECT rect;

	if (FAILED(frameTexture->LockRect(0, &rect, nullptr, 0)))
        return;

    if (rect.Pitch != 0)
    {
		assert(width * height * bytesPerPixel == rect.size);
		cv::Mat img(height, width, CV_8UC4);
		memcpy(img.ptr<short>(), rect.pBits, rect.size);
	    cv::imshow(reconstructionWindow, img);
    }

    frameTexture->UnlockRect(0);
    frameCounter++;
}

HRESULT KinectScannerFusion::ResetVolume()
{
    if (nullptr == voxelVolume)
        return E_FAIL;

    SetIdentityMatrix(worldToCameraTransform);
    lostFrameCounter = 0;
    frameCounter = 0;
    startTime = timer.AbsoluteTime();

	if (FAILED(voxelVolume->ResetReconstruction(&worldToCameraTransform, nullptr)))
	{
		SetErrorMessage(L"Failed to reset reconstruction.");
		return E_FAIL;
	}
		
	trackingFailed = false;

	return S_OK;
}

void KinectScannerFusion::SetErrorMessage(WCHAR * szMessage)
{
	std::wcout << szMessage << std::endl;
}
