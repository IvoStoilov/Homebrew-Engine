#pragma once
class Texture;
class Font
{
private:
    struct FontType
    {
        float left, right;
        int32_t size;
   };

    struct VertexType
    {
        vec4 m_Position;
        vec2 m_UV;
    };

public:
    Font();
    ~Font();

    bool Initialize(ID3D11Device* device, const std::string& fontDataFilename, const std::string& fontTexturePaths);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    void BuildVertexArray(void* outVertexArray, const std::string& textToRender, float drawX, float drawY);

private:
    bool LoadFontData(const std::string& fontDataFilename);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device* device, const std::string& fontTexturePath);
    void ReleaseTexture();

private:
    FontType* m_Font;
    Texture* m_Texture;
};