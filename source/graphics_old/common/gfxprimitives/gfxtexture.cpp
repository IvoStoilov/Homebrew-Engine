#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxtexture.h>

#include <extern/DirectXTK/Inc/WICTextureLoader.h>

GfxTexture::GfxTexture(ComPtr<ID3D11Device>& device, const String& texturePath, ShaderTarget shaderTarget)
    : m_ShaderTarget(shaderTarget)
{
    std::wstring widestr = std::wstring(texturePath.begin(), texturePath.end());
    constexpr u32 NO_CPU_ACCESS_FLAGS = 0u;
    constexpr u32 NO_MISC_FLAGS = 0u;
    constexpr ID3D11Resource** NO_RESOURCE = nullptr;
    popGfxVerify(DirectX::CreateWICTextureFromFileEx(
        device.Get(), 
        widestr.c_str(), 
        0, 
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 
        NO_CPU_ACCESS_FLAGS,
        NO_MISC_FLAGS,
        DirectX::WIC_LOADER_DEFAULT,
        NO_RESOURCE,
        &m_Texture));
}

void GfxTexture::Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const
{
    switch (m_ShaderTarget)
    {
        case ShaderTarget::Vertex: deviceContext->VSSetShaderResources(0, 1, m_Texture.GetAddressOf()); break;
        case ShaderTarget::Pixel: deviceContext->PSSetShaderResources(0, 1, m_Texture.GetAddressOf()); break;
        default:
            popAssert(false, "Shader Type not supported");
    }
}
