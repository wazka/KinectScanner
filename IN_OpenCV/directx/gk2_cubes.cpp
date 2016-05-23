#include "gk2_cubes.h"
#include "gk2_window.h"
#include "gk2_utils.h"
#include <array>

#include "../TetrahedronMarcher.h"

using namespace std;
using namespace gk2;
using namespace DirectX;

#define checkKeyPress(curr, prev, code) (curr.isKeyDown(code) && (!prev.isKeyDown(code)))

Cubes::Cubes(HINSTANCE hInstance, KinectScanData &data)
	: ApplicationBase(hInstance), m_camera(0.01f, 100.0f, 15.0f), scanData(data)
{
}

Cubes::~Cubes()
{

}

void* Cubes::operator new(size_t size)
{
	return Utils::New16Aligned(size);
}

void Cubes::operator delete(void* ptr)
{
	Utils::Delete16Aligned(ptr);
}

void Cubes::InitializeConstantBuffers()
{
	m_projCB.reset(new CBMatrix(m_device));
	m_viewCB.reset(new CBMatrix(m_device));
	m_worldCB.reset(new CBMatrix(m_device));
    m_surfaceColorCB.reset(new ConstantBuffer<XMFLOAT4>(m_device));
}

void Cubes::InitializeCamera()
{
	auto s = getMainWindow()->getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	m_projMtx = XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f);
	m_projCB->Update(m_context, m_projMtx);
	UpdateCamera();
}

void Cubes::InitializeTextures()
{
}

void Cubes::CreateScene()
{
}

void Cubes::InitializeRenderStates()
{
}

bool Cubes::LoadContent()
{
	InitializeConstantBuffers();
	InitializeCamera();
	InitializeTextures();
	InitializeRenderStates();
	CreateScene();

    m_solidEffect.reset(new SolidEffect(m_device, m_layout));
    m_solidEffect->SetProjMtxBuffer(m_projCB);
    m_solidEffect->SetViewMtxBuffer(m_viewCB);
    m_solidEffect->SetWorldMtxBuffer(m_worldCB);
    m_solidEffect->SetSurfaceColorBuffer(m_surfaceColorCB);
    
    int width = scanData.width;
    int height = scanData.height;
    int length = scanData.length;

    TetrahedronMarcher marcher;
    marcher.march(scanData.data, width, height, length);
    m_mesh = marcher.getMesh(m_device);

    m_mesh.setWorldMatrix(XMMatrixTranslation(-width / 2.0f, -height / 2.0f, -length / 2.0f) *
        XMMatrixScaling(10.0f/width, -10.0f/height, 10.0f/length));

    D3D11_RASTERIZER_DESC rsDesc = m_device.DefaultRasterizerDesc();
    rsDesc.CullMode = D3D11_CULL_NONE;
    m_rsCullNone = m_device.CreateRasterizerState(rsDesc);

	return true;
}

void Cubes::UnloadContent()
{
}

void Cubes::UpdateCamera() const
{
	m_viewCB->Update(m_context, m_camera.GetViewMatrix());
}

void Cubes::Update(float dt)
{
	static MouseState prevMouseState;
	MouseState currentMouseState;

	if (m_mouse->GetState(currentMouseState))
	{
		bool change = true;
		if (prevMouseState.isButtonDown(0))
		{
			auto d = currentMouseState.getMousePositionChange();
			m_camera.Rotate(d.y/500.f, d.x/500.f);
		}
		else if (prevMouseState.isButtonDown(1))
		{
			auto d = currentMouseState.getMousePositionChange();
			m_camera.Zoom(d.y/10.0f);
		}
		else
			change = false;
		prevMouseState = currentMouseState;
		if (change)
			UpdateCamera();
	}
}

void Cubes::DrawScene()
{
}

void Cubes::Render()
{
	if (m_context == nullptr)
		return;

	ResetRenderTarget();
	m_projCB->Update(m_context, m_projMtx);
	UpdateCamera();

	float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_context->ClearRenderTargetView(m_backBuffer.get(), clearColor);
	m_context->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_solidEffect->Begin(m_context);
    m_context->RSSetState(m_rsCullNone.get());
    m_worldCB->Update(m_context, m_mesh.getWorldMatrix());
    m_mesh.Render(m_context);
    m_solidEffect->End();

	m_swapChain->Present(0, 0);
}
