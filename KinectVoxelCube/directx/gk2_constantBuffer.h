#ifndef __GK2_CONSTANT_BUFFER_H_
#define __GK2_CONSTANT_BUFFER_H_

#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include "gk2_deviceHelper.h"

namespace gk2
{
	class ConstantBufferBase
	{
	public:
		const std::shared_ptr<ID3D11Buffer>& getBufferObject() const { return m_bufferObject; }
		ConstantBufferBase(const ConstantBufferBase& right) = delete;

	protected:
		ConstantBufferBase(DeviceHelper& device, unsigned int dataSize, unsigned int dataCount);

		void Update(const std::shared_ptr<ID3D11DeviceContext>& context, const void* dataPtr, unsigned int dataCount);
		
		void Map(const std::shared_ptr<ID3D11DeviceContext>& context);
		void* get() const;
		void Unmap(const std::shared_ptr<ID3D11DeviceContext>& context);

		int m_mapped;
		unsigned int m_dataSize;
		unsigned int m_dataCount;
		std::shared_ptr<ID3D11Buffer> m_bufferObject;
		D3D11_MAPPED_SUBRESOURCE m_resource;

	private:
		ConstantBufferBase& operator=(const ConstantBufferBase& right) { return *this; }
	};

	template<typename T, unsigned int N = 1>
	class ConstantBuffer : public ConstantBufferBase
	{
	public:
		ConstantBuffer(DeviceHelper& device)
			: ConstantBufferBase(device, sizeof(T), N)
		{ }
		ConstantBuffer(const ConstantBuffer<T, N>& right) = delete;

		void Update(const std::shared_ptr<ID3D11DeviceContext>& context, const T& data)
		{
			return ConstantBufferBase::Update(context, reinterpret_cast<const void*>(&data), 1);
		}

		void Update(const std::shared_ptr<ID3D11DeviceContext>& context, const T* data)
		{
			return ConstantBufferBase::Update(context, reinterpret_cast<const void*>(data), N);
		}

		void Map(const std::shared_ptr<ID3D11DeviceContext>& context) { ConstantBufferBase::Map(context); }
		T* get() { return reinterpret_cast<T*>(ConstantBufferBase::get()); }
		void Unmap(const std::shared_ptr<ID3D11DeviceContext>& context) { ConstantBufferBase::Unmap(context); }

	private:
		ConstantBuffer<T, N>& operator=(const ConstantBuffer<T, N>& right) { return *this; }
	};
	
	typedef ConstantBuffer<DirectX::XMMATRIX> CBMatrix;
}


#endif __GK2_CONSTANT_BUFFER_H_
