#ifndef __GK2_DEVICE_HELPER_H_
#define __GK2_DEVICE_HELPER_H_

#include <memory>
#include <d3d11.h>
#include <string>
#include <vector>

namespace gk2
{
	class DeviceHelper
	{
	public:
		DeviceHelper(const std::shared_ptr<ID3D11Device>& deviceObject = nullptr);
		DeviceHelper(const DeviceHelper& right);

		DeviceHelper& operator =(const DeviceHelper& right);

		const std::shared_ptr<ID3D11Device>& getDeviceObject() const { return m_deviceObject; }
		void setDeviceObject(const std::shared_ptr<ID3D11Device>& deviceObject) { m_deviceObject = deviceObject; }

		/*std::shared_ptr<ID3DBlob> CompileD3DShader(const std::wstring& filePath, const std::string&  entry,
												   const std::string&  shaderModel);*/
		static std::vector<BYTE> LoadByteCode(const std::wstring& fileName);
		std::shared_ptr<ID3D11VertexShader> CreateVertexShader(const std::vector<BYTE>& byteCode) const;
		std::shared_ptr<ID3D11GeometryShader> CreateGeometryShader(const std::vector<BYTE>& byteCode) const;
		std::shared_ptr<ID3D11PixelShader> CreatePixelShader(const std::vector<BYTE>& byteCode) const;
		std::shared_ptr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout,
															 unsigned int layoutElements,
															 const std::vector<BYTE>& vsByteCode) const;

		template<typename T>
		std::shared_ptr<ID3D11InputLayout> CreateInputLayout(const std::vector<BYTE>& vsByteCode)
		{
			return CreateInputLayout(T::Layout, T::LayoutElements, vsByteCode);
		}

		template<typename T>
		std::shared_ptr<ID3D11Buffer> CreateVertexBuffer(const std::vector<T>& vertices)
		{
			return _CreateBufferInternal(reinterpret_cast<const void*>(vertices.data()), sizeof(T) * vertices.size(),
				D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT);
		}

		template<typename T>
		std::shared_ptr<ID3D11Buffer> CreateVertexBuffer(unsigned int count, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) const
		{
			return _CreateBufferInternal(nullptr, sizeof(T) * count, D3D11_BIND_VERTEX_BUFFER, usage);
		}

		template<typename T>
		std::shared_ptr<ID3D11Buffer> CreateVertexBuffer(const T* vertices, unsigned int count,
														 D3D11_USAGE usage = D3D11_USAGE_DEFAULT)
		{
			return _CreateBufferInternal(reinterpret_cast<const void*>(vertices), sizeof(T) * count,
				D3D11_BIND_VERTEX_BUFFER, usage);
		}

		std::shared_ptr<ID3D11Buffer> CreateIndexBuffer(const std::vector<unsigned int> indices) const
		{
			return _CreateBufferInternal(reinterpret_cast<const void*>(indices.data()),
				sizeof(unsigned int) * indices.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT);
		}

		std::shared_ptr<ID3D11Buffer> CreateIndexBuffer(unsigned int count, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) const
		{
			return _CreateBufferInternal(nullptr, sizeof(unsigned short) * count, D3D11_BIND_INDEX_BUFFER, usage);
		}

		std::shared_ptr<ID3D11Buffer> CreateIndexBuffer(const unsigned short* indices, unsigned int count,
														D3D11_USAGE usage = D3D11_USAGE_DEFAULT) const
		{
			return _CreateBufferInternal(reinterpret_cast<const void*>(indices), sizeof(unsigned short) * count,
				D3D11_BIND_INDEX_BUFFER, usage);
		}

		std::shared_ptr<ID3D11Buffer> CreateBuffer(const D3D11_BUFFER_DESC& desc, const void* pData = nullptr) const;
		static D3D11_TEXTURE2D_DESC DefaultTexture2DDesc();
		std::shared_ptr<ID3D11Texture2D> CreateTexture2D(const D3D11_TEXTURE2D_DESC& desc) const;
		static D3D11_SHADER_RESOURCE_VIEW_DESC DefaultShaderResourceDesc();
		std::shared_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(
																	const std::shared_ptr<ID3D11Texture2D>& texture) const;
		std::shared_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(
																	const std::shared_ptr<ID3D11Texture2D>& texture,
																	const D3D11_SHADER_RESOURCE_VIEW_DESC& desc) const;
		std::shared_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(const std::wstring& fileName,
			const std::shared_ptr<ID3D11DeviceContext>& context = nullptr) const;
		static D3D11_SAMPLER_DESC DefaultSamplerDesc();
		std::shared_ptr<ID3D11SamplerState> CreateSamplerState(const D3D11_SAMPLER_DESC& desc) const;
		std::shared_ptr<ID3D11Texture2D> CreateDepthStencilTexture(SIZE size) const;
		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(
													std::shared_ptr<ID3D11Texture2D> backBufferTexture) const;
		static D3D11_DEPTH_STENCIL_VIEW_DESC DefaultDepthStencilViewDesc();
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(
																std::shared_ptr<ID3D11Texture2D> depthStencilTexture,
																D3D11_DEPTH_STENCIL_VIEW_DESC& desc) const;
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(
																std::shared_ptr<ID3D11Texture2D> depthStencilTexture) const;
		static D3D11_DEPTH_STENCIL_DESC DefaultDepthStencilDesc();
		std::shared_ptr<ID3D11DepthStencilState> CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc) const;
		static D3D11_RASTERIZER_DESC DefaultRasterizerDesc();
		std::shared_ptr<ID3D11RasterizerState> CreateRasterizerState(const D3D11_RASTERIZER_DESC& desc) const;
		static D3D11_BLEND_DESC DefaultBlendDesc();
		std::shared_ptr<ID3D11BlendState> CreateBlendState(const D3D11_BLEND_DESC& desc) const;

	private:
		std::shared_ptr<ID3D11Device> m_deviceObject;

		std::shared_ptr<ID3D11Buffer> _CreateBufferInternal(const void* pData, size_t byteWidth,
			D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage) const;
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(
																std::shared_ptr<ID3D11Texture2D> depthStencilTexture,
																D3D11_DEPTH_STENCIL_VIEW_DESC* desc) const;
		std::shared_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(
																		const std::shared_ptr<ID3D11Texture2D>& texture,
																		const D3D11_SHADER_RESOURCE_VIEW_DESC* desc) const;
	};
}

#endif __GK2_DEVICE_HELPER_H_
