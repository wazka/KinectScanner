#include "gk2_mesh.h"
#include "gk2_utils.h"

using namespace gk2;
using namespace std;
using namespace DirectX;

Mesh::Mesh(shared_ptr<ID3D11Buffer> vb, unsigned int stride, shared_ptr<ID3D11Buffer> ib, unsigned int indicesCount)
	: m_vertexBuffer(vb), m_indexBuffer(ib), m_stride(stride), m_indicesCount(indicesCount)
{
	m_worldMtx = XMMatrixIdentity();
}

Mesh::Mesh()
	: m_stride(0), m_indicesCount(0)
{
	m_worldMtx = XMMatrixIdentity();
}

Mesh::Mesh(const Mesh& right)
	: m_vertexBuffer(right.m_vertexBuffer), m_indexBuffer(right.m_indexBuffer),
	  m_stride(right.m_stride), m_indicesCount(right.m_indicesCount)
{
	m_worldMtx = XMMatrixIdentity();
}

void* Mesh::operator new(size_t size)
{
	return Utils::New16Aligned(size);
}

void Mesh::operator delete(void* ptr)
{
	Utils::Delete16Aligned(ptr);
}

Mesh& Mesh::operator =(const Mesh& right)
{
	m_vertexBuffer = right.m_vertexBuffer;
	m_stride = right.m_stride;
	m_indexBuffer = right.m_indexBuffer;
	m_indicesCount = right.m_indicesCount;
	m_worldMtx = right.m_worldMtx;
	return *this;
}

void Mesh::Render(const shared_ptr<ID3D11DeviceContext>& context) const
{
	if (!m_vertexBuffer || !m_indexBuffer || !m_indicesCount)
		return;
    // CHANGED FROM 16 to 32!
	context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
	auto b = m_vertexBuffer.get();
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &b, &m_stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(m_indicesCount, 0, 0);
}
