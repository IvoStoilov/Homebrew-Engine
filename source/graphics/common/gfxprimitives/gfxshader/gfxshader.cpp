#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxshader/gfxshader.h>

void GfxShader::Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const
{
    for (const SharedPtr<GfxBindableResource>& cbuffer : m_ConstBuffers)
    {
        cbuffer->Bind(deviceContext);
    }
}
