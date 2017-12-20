#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdint.h>

#include <string>
#include "renderer\texture.h"

class Texture;
class D3D11Model
{
private:
    struct VertexType
    {
        D3DXVECTOR4 m_Position;
        D3DXVECTOR2 m_UV;
        D3DXVECTOR3 m_Normal;
    };

public:
    D3D11Model();
    ~D3D11Model();

    bool Initialize(ID3D11Device* device, std::string& texturePath);
    void Shutdown();
    void Render(ID3D11DeviceContext* device);
    
    inline int GetIndexCount() { return m_IndexCount; }
    inline ID3D11ShaderResourceView* GetTexture() { return m_Texture->GetTexture(); }
private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* device);

    bool LoadTexture(ID3D11Device* device, std::string& texturePath);
    void ReleaseTexture();

private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    uint32_t m_IndexCount;
    uint32_t m_VertexCount;

    Texture* m_Texture;
};