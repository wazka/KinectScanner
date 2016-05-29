#pragma once
#include <vector>
#include <opencv2/core/core.hpp>

struct VoxelData
{
	unsigned int width, height, length;
	std::vector<bool> data;

	inline bool getVoxel(int x, int y, int z) {
		return data[x + y * width + z * width * height];
	}
	inline void setVoxel(int x, int y, int z, bool val) {
		data[x + y * width + z * width * height] = val;
	}
};

class Reconstruction
{
public:
	Reconstruction();
	~Reconstruction();

	VoxelData GetVoxelGrid(std::vector<std::pair<cv::Mat, cv::Mat>> frames);

private:
	VoxelData grid;

	void InitVoxelGrid();
	void CutGridWithDepthFromFrame(cv::Mat &frame);
	void ConvertFrameDepthToDepthFloatAndMirror(cv::Mat &frame);
};

