#ifndef __GK2_SOLID_EFFECT_H_
#define __GK2_SOLID_EFFECT_H_

#include "gk2_effectBase.h"

namespace gk2
{
    class SolidEffect : public EffectBase
    {
    public:
        SolidEffect(DeviceHelper& device, std::shared_ptr<ID3D11InputLayout>& layout,
            std::shared_ptr<ID3D11DeviceContext> context = nullptr);

        void SetSurfaceColorBuffer(const std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>>& surfaceColor);

    protected:
        void SetVertexShaderData() override;
        void SetPixelShaderData() override;

    private:
        static const std::wstring ShaderName;

        std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>> m_surfaceColorCB;
    };
}

#endif __GK2_SOLID_EFFECT_H_
