#include "gk2_effectBase.h"
#include "gk2_vertices.h"

using namespace std;
using namespace gk2;

EffectBase::EffectBase(shared_ptr<ID3D11DeviceContext> context /* = nullptr */)
	: m_context(context)
{
}

void EffectBase::SetWorldMtxBuffer(const shared_ptr<CBMatrix>& world)
{
	if (world != nullptr)
		m_worldCB = world;
}

void EffectBase::SetViewMtxBuffer(const shared_ptr<CBMatrix>& view)
{
	if (view != nullptr)
		m_viewCB = view;
}

void EffectBase::SetProjMtxBuffer(const shared_ptr<CBMatrix>& proj)
{
	if (proj != nullptr)
		m_projCB = proj;
}


void EffectBase::Begin(shared_ptr<ID3D11DeviceContext> context /* = nullptr */)
{
	if (context != nullptr && context != m_context)
		m_context = context;
	if (m_context == nullptr)
		return;
	m_context->VSSetShader(m_vs.get(), nullptr, 0);
	m_context->PSSetShader(m_ps.get(), nullptr, 0);
	m_context->IASetInputLayout(m_layout.get());
	SetVertexShaderData();
	SetPixelShaderData();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
void EffectBase::End()
{

}

void EffectBase::Initialize(DeviceHelper& device, shared_ptr<ID3D11InputLayout>& layout, const wstring& shaderFile)
{
	auto vsByteCode = device.LoadByteCode(shaderFile + L"VS.cso");
	auto psByteCode = device.LoadByteCode(shaderFile + L"PS.cso");
	m_vs = device.CreateVertexShader(vsByteCode);
	m_ps = device.CreatePixelShader(psByteCode);
	if (layout == nullptr)
	{
		m_layout = device.CreateInputLayout<VertexPosNormal>(vsByteCode);
		layout = m_layout;
	}
	else
		m_layout = layout;
}