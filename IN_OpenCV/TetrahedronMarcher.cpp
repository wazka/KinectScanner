#include "TetrahedronMarcher.h"
#include "directx/gk2_mesh.h"
#include "directx/gk2_deviceHelper.h"
#include "directx/gk2_vertices.h"

using namespace std;
using namespace gk2;
using namespace DirectX;

TetrahedronMarcher::TetrahedronMarcher(): _width(0), _height(0), _length(0)
{}

TetrahedronMarcher::~TetrahedronMarcher() {}

void TetrahedronMarcher::march(vector<bool> grid, int width, 
    int height, int length)
{
    assert(grid.size() == (width*height*length));
    _grid = grid;
    _width = width;
    _height = height;
    _length = length;

    for (auto z = 0; z < length - 1; ++z)
    for (auto y = 0; y < height - 1; ++y)
    for (auto x = 0; x < width - 1; ++x)
        processCube(x, y, z);
    
}

Mesh TetrahedronMarcher::getMesh(DeviceHelper &device) const
{
    assert(_positions.size() > 0);
    assert(_indices.size() > 0);

    vector<XMFLOAT3> normals(_positions.size());
    for (auto i = 0; i < _indices.size(); i += 3)
    {
        auto v1 = XMLoadFloat3(&_positions[_indices[i + 0]]);
        auto v2 = XMLoadFloat3(&_positions[_indices[i + 1]]);
        auto v3 = XMLoadFloat3(&_positions[_indices[i + 2]]);

        auto e1 = XMVector3Normalize(v2 - v1);
        auto e2 = XMVector3Normalize(v3 - v1);
        auto normal = XMVector3Cross(e1, e2);

        for (auto j = 0; j < 3; ++j)
            XMStoreFloat3(&normals[_indices[i + j]], normal);
    }

    vector<VertexPosNormal> vertices(_positions.size());
    for (auto i = 0; i < _positions.size(); ++i)
    {
        vertices[i].Pos = _positions[i];
        vertices[i].Normal = normals[i];
    }

    return Mesh(device.CreateVertexBuffer(vertices), sizeof(VertexPosNormal),
        device.CreateIndexBuffer(_indices), _indices.size());
}

void TetrahedronMarcher::processCube(int x, int y, int z)
{
    const auto NUM_TETRAHEDRONS = 6;

    //    6-------7
    //   /|      /|
    //  / |     / |
    // 2-------3  |
    // |  4----|--5
    // | /     | /
    // |/      |/
    // 0-------1

    const int TETRAHEDRONS_INDICES[NUM_TETRAHEDRONS][4] = 
    {
        { 0, 1, 3, 4 },
        { 0, 3, 4, 6 },
        { 0, 2, 3, 6 },
        { 1, 4, 5, 7 },
        { 1, 3, 4, 7 },
        { 3, 4, 6, 7 },
    };

    for (auto i = 0; i < NUM_TETRAHEDRONS; ++i)
    {
        vector<Array3DCoord> coords(4);
        for (auto j = 0; j < 4; ++j)
        {
            auto maskedDisplacement = TETRAHEDRONS_INDICES[i][j];
            auto dx = maskedDisplacement & 0b001;
            auto dy = (maskedDisplacement & 0b010) >> 1;
            auto dz = (maskedDisplacement & 0b100) >> 2;

            assert(dx == 0 || dx == 1);
            assert(dy == 0 || dy == 1);
            assert(dz == 0 || dz == 1);

            coords[j] = Array3DCoord(x + dx, y + dy, z + dz);
        }
        
        processTetrahedra(coords);
    }
    
}

int firstBitSet(unsigned int mask, int limit)
{
    assert(limit <= 32);
    for (auto i = 0; i < limit; ++i)
        if (mask&(1 << i))
            return i;
    return -1;
}

void TetrahedronMarcher::processTetrahedra(const vector<Array3DCoord> &vertices)
{
    assert(vertices.size() == 4);
    auto mask = buildTetrahedraMask(vertices);
    if (mask == 0 || mask == 0b1111)
        return;

    int p[2], k = 0;
    switch (mask)
    {
    case 0b0001:
    case 0b0010:
    case 0b0100:
    case 0b1000:
        buildSingleTriangle(vertices, firstBitSet(mask, 4));
        break;
    case 0b1110:
    case 0b1101:
    case 0b1011:
    case 0b0111:
        buildSingleTriangle(vertices, firstBitSet(~mask&0b1111, 4));
        break;
    case 0b1100:
    case 0b1010:
    case 0b1001:
    case 0b0110:
    case 0b0101:
    case 0b0011:
        for (auto i = 0; i < 4; ++i)
            if (mask & (1 << i)) 
                p[k++] = i;
        assert(k == 2);
        buildTwoTriangles(vertices, make_pair(p[0], p[1]));
        break;
    }
}

unsigned int TetrahedronMarcher::buildTetrahedraMask(const vector<Array3DCoord>& vertices)
{
    assert(vertices.size() == 4);
    unsigned int mask = 0;
    for (auto i = 0; i < 4; ++i)
        mask |= (_grid[getArrayCoordinate(vertices[i])] ? 1 : 0) << i;
    return mask;
}

void TetrahedronMarcher::buildSingleTriangle(const vector<Array3DCoord>& vertices, int mainVertex)
{
    assert(vertices.size() == 4);

    XMFLOAT3 v[3];
    int filled = 0;

    for (auto i = 0; i < 4; ++i)
    {
        if (i == mainVertex) continue;
        // todo: Check face winding.
        v[filled++] = XMFLOAT3(
            (vertices[mainVertex].x + vertices[i].x) * 0.5f,
            (vertices[mainVertex].y + vertices[i].y) * 0.5f,
            (vertices[mainVertex].z + vertices[i].z) * 0.5f
        );
    }

    // todo: Repeating vertices between tetrahedrons.
    auto baseIndex = _positions.size();
    for (auto i = 0; i < 3; ++i)
    {
        _positions.push_back(v[i]);
        _indices.push_back(baseIndex + i);
    }
}

void TetrahedronMarcher::buildTwoTriangles(const vector<Array3DCoord>& vertices, pair<int,int> edge)
{
    assert(vertices.size() == 4);

    XMFLOAT3 v[4];
    int filled = 0;

    for (auto i = 0; i < 4; ++i)
    {
        if (i == edge.first || i == edge.second) continue;

        v[filled++] = XMFLOAT3(
            (vertices[edge.first].x + vertices[i].x) * 0.5f,
            (vertices[edge.first].y + vertices[i].y) * 0.5f,
            (vertices[edge.first].z + vertices[i].z) * 0.5f
        );

        v[filled++] = XMFLOAT3(
            (vertices[edge.second].x + vertices[i].x) * 0.5f,
            (vertices[edge.second].y + vertices[i].y) * 0.5f,
            (vertices[edge.second].z + vertices[i].z) * 0.5f
        );
    }

    assert(filled == 4);

    auto baseIndex = _positions.size();
    for (auto i = 0; i < 4; ++i)
        _positions.push_back(v[i]);

    const int triIndices[6] = { 0, 1, 2, 1, 3, 2 };
    for (auto i = 0; i < 6; ++i)
        _indices.push_back(baseIndex + triIndices[i]);
}

int TetrahedronMarcher::getArrayCoordinate(const Array3DCoord& coord) const
{
    return coord.x + coord.y * _width + coord.z * _width * _height;
}
