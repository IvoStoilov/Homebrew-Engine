#pragma once
#include <system\modelloader.h>

class Texture;
class Model3D
{
private:
    struct VertexType
    {
        vec4 m_Position;
        vec3 m_Normal;
        vec2 m_UV;
    };

public:
    Model3D();
    ~Model3D();

    bool Initialize(ID3D11Device* device, std::string& modelPath, std::string& texturePath);
    void Shutdown();
    void Render(ID3D11DeviceContext* device);
    
    inline int GetIndexCount() { return m_IndexCount; }
    SharedPtr<Texture> GetTexture() const { return m_Texture; }

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* device);

    bool LoadTexture(ID3D11Device* device, std::string& texturePath);
    
private:
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;

    std::vector<ModelLoader::VertexData> m_VertexData;

    uint32_t m_IndexCount;
    uint32_t m_VertexCount;

    SharedPtr<Texture> m_Texture;
};