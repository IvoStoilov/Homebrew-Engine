#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdint.h>

class Font;
class FontShader;
class Text
{
private:
    struct SentenceType
    {
        ID3D11Buffer* vertexBuffer;
        ID3D11Buffer* indexBuffer;

        int vertexCount;
        int indexCount;
        int maxLength;

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

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint16_t screenWidth, uint16_t screenHeight);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);

private:
    bool InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device);
    bool UpdateSentence(SentenceType* sentence, char* text, int posX, int posY, float r, float g, float b, ID3D11DeviceContext* deviceContext);
    void ReleaseSentence(SentenceType**);
    bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
    Font* m_Font;
    FontShader* m_FontShader;
    uint16_t m_ScreenWidth;
    uint16_t m_ScreenHeight;

    SentenceType* m_Text1;
    SentenceType* m_Text2;
};