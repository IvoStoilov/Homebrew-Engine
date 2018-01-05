#pragma once
class Font;
class FontShader;
class Text
{
private:
    struct SentenceType
    {
        ID3D11Buffer* vertexBuffer;
        ID3D11Buffer* indexBuffer;

        uint32_t vertexCount;
        uint32_t indexCount;
        uint32_t maxLength;

        float red, green, blue;
    };

    struct VertexType
    {
        D3DXVECTOR4 position;
        D3DXVECTOR2 texture;
    };

public:
    Text();
    ~Text();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint16_t screenWidth, uint16_t screenHeight, D3DXMATRIX baseViewMatrix);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);

private:
    bool InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device);
    bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
    void ReleaseSentence(SentenceType**);
    bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
    Font* m_Font;
    FontShader* m_FontShader;
    uint16_t m_ScreenWidth;
    uint16_t m_ScreenHeight;
    D3DXMATRIX m_BaseViewMatrix;

    SentenceType* m_Text1;
    SentenceType* m_Text2;
};