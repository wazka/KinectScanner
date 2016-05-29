#include "FakeVoxelGridProvider.h"
using namespace std;

vector<bool> createVoxelizedSphereInGrid(int width, int height, int length, float sphereRadius)
{
    vector<bool> grid(width*height*length);

    float spherePosX = 0.5f * width;
    float spherePosY = 0.5f * height;
    float spherePosZ = 0.5f * length;

    for (auto z = 0; z < length; ++z)
    for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
    {
        auto dx = (spherePosX - x);
        auto dy = (spherePosY - y);
        auto dz = (spherePosZ - z);
        auto lenSq = dx*dx + dy*dy + dz*dz;
        auto len = sqrt(lenSq);
        grid[x + width*y + width*height*z] = len < sphereRadius;
    }

    return grid;
}