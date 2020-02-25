#pragma once
class Texture;
class Model2D
{
private:
    struct VertexType
    {
        vec4 m_Position;
        vec2 m_UV;
    };

public:
    Model2D();
    ~Model2D();

    bool Initialize(ID3D11Device* device, uint32_t screenWidth, uint32_t screenHeight, std::string& texturePath, uint32_t modelWidth, uint32_t modelHeight);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, int32_t posX, int32_t posY);

    inline int GetIndexCount() { return m_IndexCount; }
    ID3D11ShaderResourceView* GetTexture();
    inline mat4x4 GetWorldMatrix() const { return m_WorldMatrix; }

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int32_t posX, int32_t poxY);
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

    bool LoadTexture(ID3D11Device* device, std::string& texturePath);
    void ReleaseTexture();

private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    uint32_t m_IndexCount;
    uint32_t m_VertexCount;

    Texture* m_Texture;

    int32_t m_ScreenWidth, m_ScreenHeight;
    int32_t m_ModelWidth, m_ModelHeight;
    int32_t m_PreviousePosX, m_PreviousePosY;

    mat4x4 m_WorldMatrix;
};