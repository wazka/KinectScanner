
#include <string>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "../KinectScanner.h"

using namespace std;
using namespace cv;

KinectScanner::KinectScanner() :
	kinectSensor(nullptr),
	depthStream(INVALID_HANDLE_VALUE),
	currentDepthFrame(480, 640, CV_16UC1),
	currentColorFrame(480, 640, CV_8UC3)
{
	width = 640;
	height = 480;
	frameSize = width*height;

	//namedWindow(previewWindow, cv::WINDOW_AUTOSIZE);

	for (int i = 1; i <= persistedSceneFrameCount; ++i) {
		depthFrameNames.push_back("depth" + std::to_string(i));
		colorFrameNames.push_back("color" + std::to_string(i));
	}
}

KinectScanner::~KinectScanner()
{
	if (kinectSensor)
	{
		kinectSensor->NuiShutdown();
		kinectSensor->Release();
	}
}


HRESULT KinectScanner::InitKinectSensor()
{
	INuiSensor * possiblySensor;
	HRESULT hr = S_OK;

	int sensorCount = 0;
	if (FAILED(NuiGetSensorCount(&sensorCount)))
	{
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
		if (SUCCEEDED(kinectSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR)))
		{
			hr = kinectSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, NULL, &depthStream);
			if (FAILED(hr))
				return E_FAIL;
			hr = kinectSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, NULL, &colorStream);
			if (FAILED(hr))
				return E_FAIL;
		}
	}

	if (nullptr == kinectSensor || FAILED(hr))
	{
		return E_FAIL;
	}

	return hr;
}

HRESULT KinectScanner::UpdateCurrentDepthFrame()
{
	NUI_IMAGE_FRAME frame;
	HRESULT result = S_OK;

	result = kinectSensor->NuiImageStreamGetNextFrame(depthStream, 0, &frame);
	if (result < 0)
		return result;

	NUI_LOCKED_RECT rect;
	frame.pFrameTexture->LockRect(0, &rect, nullptr, 0);
	if (rect.Pitch != 0)
	{
		assert(width*height * 2 == rect.size);
		memcpy(currentDepthFrame.ptr<short>(), rect.pBits, rect.size);
		// currentDepthFrame = currentDepthFrame / 8;
		result = S_OK;
	}
	else
		result = E_FAIL;

	frame.pFrameTexture->UnlockRect(0);
	kinectSensor->NuiImageStreamReleaseFrame(depthStream, &frame);

	return result;
}

HRESULT KinectScanner::UpdateCurrentColorFrame()
{
	NUI_IMAGE_FRAME frame;
	HRESULT result = S_OK;

	result = kinectSensor->NuiImageStreamGetNextFrame(colorStream, 0, &frame);
	if (result < 0)
		return result;

	NUI_LOCKED_RECT rect;
	frame.pFrameTexture->LockRect(0, &rect, nullptr, 0);
	if (rect.Pitch != 0)
	{
		assert(width*height * 4 == rect.size);
		Mat temp(480, 640, CV_8UC4);
		memcpy(temp.ptr<short>(), rect.pBits, rect.size);
		cvtColor(temp, temp, CV_RGBA2RGB);
		flip(temp, currentColorFrame, 1);
		result = S_OK;
	}
	else
		result = E_FAIL;

	frame.pFrameTexture->UnlockRect(0);
	kinectSensor->NuiImageStreamReleaseFrame(colorStream, &frame);

	return result;
}

Mat KinectScanner::GetCurrentDepthFrame()
{
	return currentDepthFrame;
}

void KinectScanner::UpdatePreview()
{
	imshow(previewWindow, currentDepthFrame);
}

void KinectScanner::PersistScene(string path)
{
	waitKey(200);
	FileStorage fs(path, FileStorage::WRITE);

	for (int i = 0; i < persistedSceneFrameCount; ++i)
	{
		UpdateCurrentDepthFrame();
		UpdateCurrentColorFrame();
		fs << depthFrameNames[i] << currentDepthFrame;
		fs << colorFrameNames[i] << currentColorFrame;

		UpdatePreview();
		waitKey(200);
	}

	fs.release();
}

vector<pair<Mat, Mat>> KinectScanner::LoadSceneFromFile(string path)
{
	vector<pair<Mat, Mat>> frames;
	FileStorage fs(path.c_str(), FileStorage::READ);

	for (int i = 0; i < persistedSceneFrameCount; ++i)
	{
		Mat depthFrame, colorFrame;
		fs[depthFrameNames[i]] >> depthFrame;
		fs[colorFrameNames[i]] >> colorFrame;
		frames.push_back(pair<Mat, Mat>(depthFrame, colorFrame));
	}

	return frames;
}
