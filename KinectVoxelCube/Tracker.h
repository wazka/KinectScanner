#pragma once
#include <vector>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

class Tracker
{
public:
	void InitTracking(Mat firstFrame, Mat secondFrame);
	void UpdateOpticalFlow(Mat nextFrame);
	Mat GetCurrentFundamentalMatrix();

	const int maxFeaturesCount = 400;
	const double qualityLevel = 0.01;
	const double minDistance = 10;

	Mat prevFrame;
	Mat nextFrame;

	vector<Point2f> featuresPrev;
	vector<Point2f> featuresNext;

	Mat opticalFlowStatus;
	Mat opticalFlowError;
};

