#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdint.h>

class D3D11Model
{
private:
    struct VertexType
    {
        D3DXVECTOR4 m_Position;
        D3DXVECTOR4 m_Color;
    };

public:
    D3D11Model();
    ~D3D11Model();

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* device);
    
    int GetIndexCount() { return m_IndexCount; }
private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* device);

private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    uint32_t m_IndexCount;
    uint32_t m_VertexCount;
};