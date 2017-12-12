#include "renderer/texture.h"
#include "extern/DirectXTK/Inc/WICTextureLoader.h"

Texture::Texture() :
    m_Texture(nullptr)
{
}

Texture::~Texture()
{}

bool Texture::Initialize(ID3D11Device* device, const std::string& texturePath)
{
    std::wstring widestr = std::wstring(texturePath.begin(), texturePath.end());
    HRESULT result = DirectX::CreateWICTextureFromFileEx(device, widestr.c_str(), 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, 0, false, nullptr, &m_Texture);

    if (FAILED(result))
        return false;

    return true;
}

void Texture::Shutdown()
{
    if (m_Texture)
        m_Texture->Release();

    m_Texture = nullptr;
}