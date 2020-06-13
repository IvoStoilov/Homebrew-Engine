#pragma once

class FontShader
{
private:
    struct MatrixBufferType
    {
        mat4x4 world;
        mat4x4 view;
        mat4x4 projection;
    };

    struct PixelBufferType
    {
        vec4 fontColor;
    };
public:
    FontShader();
    ~FontShader();

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, mat4x4& worldMatrix, mat4x4& viewMatrix, mat4x4& projectionMatrix, ID3D11ShaderResourceView* texture, vec4& pixelColor);

private:
    bool InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath);
    void ShutdownShader();

    bool SetShaderParameters(ID3D11DeviceContext* deviceContext, mat4x4 worldMatrix, mat4x4 viewMatrix, mat4x4 projectionMatrix, ID3D11ShaderResourceView* texture, vec4& pixelColor);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    ID3D11InputLayout* m_Layout;
    ID3D11Buffer* m_MatrixBuffer;
    ID3D11Buffer* m_FontColorBuffer;

    ID3D11SamplerState* m_SampleState;
};