#include <Windows.h>
#include <NuiApi.h>
#include "Reconstruction.h"
#include "Tracker.h"

using namespace std;
using namespace cv;

const int VOXELS_PER_METER = 64;
const int VOXEL_COUNT_X = 64;
const int VOXEL_COUNT_Y = 64;
const int VOXEL_COUNT_Z = 64;
const float VOXEL_SIZE = 1.0f / VOXELS_PER_METER;
const float CUBE_WIDTH = (float)VOXEL_COUNT_X / VOXELS_PER_METER; // meters
const float CUBE_HEIGHT = (float)VOXEL_COUNT_Y / VOXELS_PER_METER; // meters
const float CUBE_LENGTH = (float)VOXEL_COUNT_Z / VOXELS_PER_METER; // meters
const float CUBE_CAMERA_DIST = 1.0f; // 1 metr
const float FOCAL_LENGTH = 2 * NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS; // sta³a jest dla rozdzielczoœci 320x240, my mamy 640x480

Reconstruction::Reconstruction()
{

}

Reconstruction::~Reconstruction()
{

}

/*
* W wyniku dostajemy wartosci odleglosci w metrach od plaszczyzny sensora
* 0 oznacza blad odczytu (brak, za blisko lub za daleko)
*/
void Reconstruction::ConvertFrameDepthToDepthFloatAndMirror(Mat &frame)
{
	Mat temp(frame.rows, frame.cols, CV_32FC1);
	for (int i = 0; i < frame.rows; ++i) {
		for (int j = 0; j < frame.cols; ++j) {
			short val = frame.at<short>(i, j);
			float val_m = (float)val / NUI_IMAGE_DEPTH_MAXIMUM * 4; // 0.8m - 4m zakres wartosci
			temp.at<float>(i, frame.cols - 1 - j) = val_m;
		}
	}
	frame = temp;
}

void Reconstruction::InitVoxelGrid()
{
	grid.width = VOXEL_COUNT_X;
	grid.height = VOXEL_COUNT_Y;
	grid.length = VOXEL_COUNT_Z;
	grid.data = vector<bool>(VOXEL_COUNT_X * VOXEL_COUNT_Y * VOXEL_COUNT_Z);
	for (int i = 0; i < VOXEL_COUNT_X; ++i)
		for (int j = 0; j < VOXEL_COUNT_Y; ++j)
			for (int k = 0; k < VOXEL_COUNT_Z; ++k)
				grid.setVoxel(i, j, k, true);
}

void Reconstruction::CutGridWithDepthFromFrame(Mat &frame)
{
	assert(frame.cols == 640 && frame.rows == 480);
	for (int i = 0; i < VOXEL_COUNT_X; ++i) {
		for (int j = 0; j < VOXEL_COUNT_Y; ++j) {
			for (int k = 0; k < VOXEL_COUNT_Z; ++k) {
				float x = (-0.5f + (float)i / VOXEL_COUNT_X) * CUBE_WIDTH;
				float y = (-0.5f + (float)j / VOXEL_COUNT_Y) * CUBE_HEIGHT;
				float z = CUBE_CAMERA_DIST + ((float)k / VOXEL_COUNT_Z) * CUBE_LENGTH;
				// Mno¿enie przez macierz kamery by uzyskaæ wspó³rzêdne afiniczne w jej uk³adzie
				// f 0 cx | x = u
				// 0 f cy | y = v
				// 0 0 1  | z = w
				int u = round(FOCAL_LENGTH * x / z + 320); // cx = 640 / 2, dzielimy przez w ¿eby uzyskaæ wspó³rzêdne na p³aszczyŸnie kamery
				int v = round(FOCAL_LENGTH * y / z + 240); // cy = 480 / 2
				// Œrodek woksela po zrzutowaniu znajduje siê na mapie g³êbokoœci
				if (u >= 0 && u < 640 && v >= 0 && v < 480) {
					float depth = frame.at<float>(v, u);
					if (depth > z)
						grid.setVoxel(i, j, k, false);
				}
			}
		}
	}
}

VoxelData Reconstruction::GetVoxelGrid(vector<pair<Mat, Mat>> frames)
{
	InitVoxelGrid();
	for (int i = 0; i < frames.size(); ++i)
		ConvertFrameDepthToDepthFloatAndMirror(frames[i].first);

	CutGridWithDepthFromFrame(frames[3].first);

	Tracker tracker;
	tracker.InitTracking(frames[3].second, frames[4].second);

	//for (int i = 2; i < frames.size(); ++i)
	//	tracker.UpdateOpticalFlow(frames[i]);


	return grid;
}