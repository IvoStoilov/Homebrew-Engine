#pragma once
class ColorShader
{
private:
    struct MatrixBufferType
    {
        mat4x4 world;
        mat4x4 view;
        mat4x4 projection;
    };

public:
    ColorShader();
    ~ColorShader();
    
    bool Initialize(ID3D11Device* device);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, mat4x4 worldMatrix, mat4x4 viewMatrix, mat4x4 projectionMatrix);

private:
    bool InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath);
    void ShutdownShader();

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, mat4x4& worldMatrix, mat4x4& viewMatrix, mat4x4& projectionMatrix);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_Layout;
    ID3D11Buffer* m_MatrixBuffer;
};