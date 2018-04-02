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

        int32_t posX;
        int32_t posY;

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

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint32_t maxLength, uint16_t screenWidth, uint16_t screenHeight, 
                    uint16_t numberOfLines = 1, int16_t posX = 0, int16_t posY = 0);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);

    bool SetText(char* text, uint16_t slot);
private:
    bool InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device);
    bool UpdateSentence(SentenceType* sentence, char* text, int posX, int posY, float r, float g, float b);
    void ReleaseSentence(SentenceType**);
    bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
    Font* m_Font;
    FontShader* m_FontShader;
    uint16_t m_ScreenWidth;
    uint16_t m_ScreenHeight;

    ID3D11DeviceContext* m_DeviceContextCache;

    uint16_t m_NumberOfLines;
    SentenceType** m_Lines;
};