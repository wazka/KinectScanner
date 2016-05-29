#pragma once

#include <opencv2\core\core.hpp>
#include <Windows.h>
#include <NuiApi.h>
#include <string>
#include <vector>

class KinectScanner
{
	const std::string previewWindow = "PreviewWindow";
	const int persistedSceneFrameCount = 50;

public:

	KinectScanner();
	~KinectScanner();

	HRESULT InitKinectSensor();
	HRESULT UpdateCurrentDepthFrame();
	HRESULT UpdateCurrentColorFrame();
	cv::Mat GetCurrentDepthFrame();
	void UpdatePreview();
	void PersistScene(std::string path);
	std::vector<std::pair<cv::Mat, cv::Mat>> LoadSceneFromFile(std::string path);

private:

	INuiSensor* kinectSensor;
	int width;
	int height;
	int frameSize;
	HANDLE depthStream;
	HANDLE colorStream;
	cv::Mat currentDepthFrame;
	cv::Mat currentColorFrame;
	std::vector<std::string> depthFrameNames;
	std::vector<std::string> colorFrameNames;
};
