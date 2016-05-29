#include "gk2_solidEffect.h"

using namespace std;
using namespace gk2;
using namespace DirectX;

const wstring SolidEffect::ShaderName = L"solid";

SolidEffect::SolidEffect(DeviceHelper& device, shared_ptr<ID3D11InputLayout>& layout,
    shared_ptr<ID3D11DeviceContext> context /* = nullptr */)
    : EffectBase(context)
{
    Initialize(device, layout, ShaderName);
}

void SolidEffect::SetSurfaceColorBuffer(const shared_ptr<ConstantBuffer<XMFLOAT4>>& surfaceColor)
{
    if (surfaceColor != nullptr)
        m_surfaceColorCB = surfaceColor;
}

void SolidEffect::SetVertexShaderData()
{
    ID3D11Buffer* vsb[3] = { m_worldCB->getBufferObject().get(), m_viewCB->getBufferObject().get(),
        m_projCB->getBufferObject().get() };
    m_context->VSSetConstantBuffers(0, 3, vsb);
}

void SolidEffect::SetPixelShaderData()
{
    ID3D11Buffer* psb[1] = { m_surfaceColorCB->getBufferObject().get() };
    m_context->PSSetConstantBuffers(0, 1, psb);
}
