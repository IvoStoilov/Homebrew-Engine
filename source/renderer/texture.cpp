#include "renderer/texture.h"

//#include <dxgi.h>
//#include <d3dcommon.h>
//#include <d3d11.h>
//#include <d3dx11tex.h>
//
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")

#include "extern/DirectXTK/Inc/WICTextureLoader.h"

Texture::Texture() :
    m_Texture(nullptr)
{
}

Texture::~Texture()
{}

bool Texture::Initialize(ID3D11Device* device, const std::string& texturePath)
{
    WCHAR* asd = (WCHAR*)texturePath.c_str();
    HRESULT result = DirectX::CreateWICTextureFromFileEx(device, asd, 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0, 0, false, nullptr, &m_Texture);

    if (FAILED(result))
        return false;

    return true;
}