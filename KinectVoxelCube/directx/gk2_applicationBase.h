#ifndef __GK2_APPLICATION_BASE_H_
#define __GK2_APPLICATION_BASE_H_

#include <d3d11.h>
#include "gk2_input.h"
#include "gk2_deviceHelper.h"

namespace gk2
{
	class Window;

	class ApplicationBase
	{
	public:
		ApplicationBase(HINSTANCE hInstance);
		virtual ~ApplicationBase();

		int Run(Window* window, int cmdShow);

		HINSTANCE getHandle() const { return m_hInstance; }
		Window* getMainWindow() const { return m_mainWindow; }

	protected:
		bool Initialize();
		int MainLoop();
		void Shutdown();

		virtual bool LoadContent();
		virtual void UnloadContent();
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

		void ResetRenderTarget() const;

		D3D_DRIVER_TYPE m_driverType;
		D3D_FEATURE_LEVEL m_featureLevel;

		DeviceHelper m_device;
		std::shared_ptr<ID3D11DeviceContext> m_context;
		std::shared_ptr<IDXGISwapChain> m_swapChain;
		std::shared_ptr<ID3D11Texture2D> m_backBufferTexture;
		std::shared_ptr<ID3D11RenderTargetView> m_backBuffer;
		std::shared_ptr<ID3D11Texture2D> m_depthStencilTexture;
		std::shared_ptr<ID3D11DepthStencilView> m_depthStencilView;
		InputHelper m_input;
		std::shared_ptr<Keyboard> m_keyboard;
		std::shared_ptr<Mouse> m_mouse;

	private:
		HINSTANCE m_hInstance;
		Window* m_mainWindow;

		void FillSwapChainDesc(DXGI_SWAP_CHAIN_DESC& desc, int width, int height) const;
		void CreateDeviceAndSwapChain(SIZE windowSize);
		void CreateBackBuffers(SIZE windowSize);
		void InitializeDirectInput();
		void SetViewPort(SIZE windowSize) const;
	};
}

#endif __GK2_APPLICATION_BASE_H_
