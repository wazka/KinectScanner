#ifndef __GK2_VERTICES_H_
#define __GK2_VERTICES_H_

#include <d3d11.h>
#include <DirectXMath.h>

namespace gk2
{
	struct VertexPos 
	{
		DirectX::XMFLOAT3 Pos;
		static const unsigned int LayoutElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Layout[LayoutElements];
	};

	struct VertexPosNormal
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		static const unsigned int LayoutElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Layout[LayoutElements];
	};
}

#endif __GK2_VERTICES_H_
