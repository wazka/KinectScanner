#ifndef __GK2_MESH_H_
#define __GK2_MESH_H_

#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>

namespace gk2
{
	class Mesh
	{
	public:
		Mesh(std::shared_ptr<ID3D11Buffer> vb, unsigned int stride,
			 std::shared_ptr<ID3D11Buffer> ib, unsigned int indicesCount);
		Mesh();
		Mesh(const Mesh& right);

		const DirectX::XMMATRIX& getWorldMatrix() const { return m_worldMtx; }
		void setWorldMatrix(const DirectX::XMMATRIX& mtx) { m_worldMtx = mtx; }
		void Render(const std::shared_ptr<ID3D11DeviceContext>& context) const;

		Mesh& operator =(const Mesh& right);

		static void* operator new(size_t size);
		static void operator delete(void* ptr);

	private:
		std::shared_ptr<ID3D11Buffer> m_vertexBuffer;
		std::shared_ptr<ID3D11Buffer> m_indexBuffer;
		unsigned int m_stride;
		unsigned int m_indicesCount;
		DirectX::XMMATRIX m_worldMtx;
	};
}

#endif __GK2_MESH_H_
