#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdint.h>
#include <vector>

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

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint16_t screenWidth, uint16_t screenHeight, int16_t posX = 0, int16_t posY = 0);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);

    bool SetText(char* text, uint16_t slot);
    bool AppendLine(char* text, uint32_t maxLength, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255);
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
    int16_t m_PosX;
    int16_t m_PosY;

    ID3D11DeviceContext* m_DeviceContextCache;
    ID3D11Device*        m_DeviceCache;

    std::vector<SentenceType*> m_Lines;
};