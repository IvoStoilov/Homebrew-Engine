#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdint.h>
#include <string>


class D3D11Shader
{
private:
    struct MatrixBufferType
    {
        D3DXMATRIX world;
        D3DXMATRIX view;
        D3DXMATRIX projection;
    };

public:
    D3D11Shader();
    ~D3D11Shader();
    
    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix);

private:
    bool InitializeShader(ID3D11Device* device, HWND hwnd, const std::string& vsPath, const std::string& psPath);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_Layout;
    ID3D11Buffer* m_MatrixBuffer;
};