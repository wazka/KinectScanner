#include "Tracker.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>


void Tracker::InitTracking(Mat firstFrame, Mat secondFrame)
{
	Mat grayFirstFrame;
	cvtColor(firstFrame, grayFirstFrame, CV_RGB2GRAY);
	nextFrame = firstFrame.clone();

	goodFeaturesToTrack(grayFirstFrame, featuresNext, maxFeaturesCount, qualityLevel, minDistance);

	//for (int i = 0; i < featuresNext.size(); ++i)
	//	circle(grayFirstFrame, featuresNext[i], 3, Scalar(255.0, 0.0, 0.0, 1.0));

	UpdateOpticalFlow(secondFrame);
}

void Tracker::UpdateOpticalFlow(Mat newFrame)
{
	prevFrame = nextFrame.clone();
	nextFrame = newFrame.clone();
	featuresPrev.clear();
	featuresPrev.insert(featuresPrev.end(), featuresNext.begin(), featuresNext.end());

	Mat grayNewFrame;
	cvtColor(newFrame, grayNewFrame, CV_RGB2GRAY);

	Mat grayOldFrame;
	cvtColor(prevFrame, grayOldFrame, CV_RGB2GRAY);

	calcOpticalFlowPyrLK(grayOldFrame, grayNewFrame, featuresPrev, featuresNext, opticalFlowStatus, opticalFlowError);

}

