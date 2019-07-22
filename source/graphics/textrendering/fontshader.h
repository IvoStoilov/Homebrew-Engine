#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdint.h>
#include <string>

class FontShader
{
private:
    struct MatrixBufferType
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

    struct PixelBufferType
    {
        D3DXVECTOR4 fontColor;
    };
public:
    FontShader();
    ~FontShader();

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR4& pixelColor);

private:
    bool InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath);
    void ShutdownShader();

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR4& pixelColor);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_Layout;
    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_FontColorBuffer;

    ID3D11SamplerState* m_SampleState;
};