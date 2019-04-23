#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <directxmath.h>
#include <stdint.h>
#include <string>

class SkydomeShader
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    struct ColorBufferType
    {
        DirectX::XMFLOAT4 apexColor;
        DirectX::XMFLOAT4 centerColor;
    };
public:
    bool Initialize(ID3D11Device* device);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, DirectX::XMFLOAT4 apexColor, DirectX::XMFLOAT4 centerColor);

private:
    bool InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath);
    void ShutdownShader();

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix, DirectX::XMFLOAT4 apexColor, DirectX::XMFLOAT4 centerColor);
    void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_Layout = nullptr;
    ID3D11Buffer* m_MatrixBuffer = nullptr;
    ID3D11Buffer* m_ColorBuffer = nullptr;
};