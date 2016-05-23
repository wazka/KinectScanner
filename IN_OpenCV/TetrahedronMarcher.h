#pragma once
#include <vector>
#include <DirectXMath.h>

namespace gk2 {
    class DeviceHelper;
    class Mesh;
}

struct Array3DCoord
{
    Array3DCoord() : x(0), y(0), z(0) {}
    Array3DCoord(int x, int y, int z) : x(x), y(y), z(z) {}

    int x;
    int y;
    int z;
};

class TetrahedronMarcher
{
public:
    TetrahedronMarcher();
    ~TetrahedronMarcher();

    void march(std::vector<bool> grid, int width, int height, int length);
    std::vector<DirectX::XMFLOAT3> getPositions() const { return _positions; }
    std::vector<unsigned int> getIndices() const { return _indices; }
    gk2::Mesh getMesh(gk2::DeviceHelper &device) const;

protected:
    void processCube(int x, int y, int z);
    void processTetrahedra(const std::vector<Array3DCoord> &vertices);
    unsigned int buildTetrahedraMask(const std::vector<Array3DCoord> &vertices);
    void buildSingleTriangle(const std::vector<Array3DCoord> &vertices, int mainVertex);
    void buildTwoTriangles(const std::vector<Array3DCoord> &vertices, 
        std::pair<int,int> edge);

    int getArrayCoordinate(const Array3DCoord &coord) const;

private:
    std::vector<bool> _grid;
    int _width, _height, _length;

    std::vector<DirectX::XMFLOAT3> _positions;
    std::vector<unsigned int> _indices;
};
