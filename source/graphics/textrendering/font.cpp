#include "graphics/textrendering/font.h"
#include "graphics/common/texture.h"

#include "system/error.h"

#include <fstream>

//Set letter spacing to 2px.
const float LETTER_SPACING = 2.f;

Font::Font():
    m_Texture(nullptr),
    m_Font(nullptr)
{}

Font::~Font()
{}

bool Font::Initialize(ID3D11Device* device, const std::string& fontDataFilename, const std::string& fontTexturePath)
{
    if (!LoadFontData(fontDataFilename))
        return false;

    if (!LoadTexture(device, fontTexturePath))
        return false;

    return true;
}

void Font::Shutdown()
{
    ReleaseTexture();
    ReleaseFontData();
}

//TODO (istoilov): Possibly on a next pass move this logic into a dedicated loader
bool Font::LoadFontData(const std::string& fontDataFilename)
{
    std::ifstream input;
    input.open(fontDataFilename);
    popAssert(!input.fail(), "Font::LoadFontData::open failed.");

    m_Font = new FontType[95];
    char temp;
    for (uint16_t i = 0; i < 95; ++i)
    {
        input.get(temp);
        while (temp != ' ')
        {
            input.get(temp);
        }
        input.get(temp);
        while (temp != ' ')
        {
            input.get(temp);
        }

        input >> m_Font[i].left;
        input >> m_Font[i].right;
        input >> m_Font[i].size;
    }

    input.close();
    return true;
}

void Font::ReleaseFontData()
{
    if (m_Font)
    {
        delete[] m_Font;
        m_Font = nullptr;
    }
}

bool Font::LoadTexture(ID3D11Device* device, const std::string& fontTexturePath)
{
    m_Texture = new Texture();
    return m_Texture->Initialize(device, fontTexturePath);
}

void Font::ReleaseTexture()
{
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
    }
}

ID3D11ShaderResourceView* Font::GetTexture()
{
    return m_Texture->GetTexture();
}

//TODO : check if it is a good idea to "believe" the ourVertexArray size is appropriate
void Font::BuildVertexArray(void* outVertexArray, const std::string& textToRender, float drawX, float drawY)
{
    float scaleWidth = 1.f;
    float scaleHeight = 1.f;
    int32_t textToRenderSize = textToRender.size();
    uint32_t index = 0;
    VertexType* vertexPtr = (VertexType*)outVertexArray;

    for (uint32_t i = 0; i < textToRenderSize; ++i)
    {
        char letter = textToRender[i] - 32;

        //If the letter is a space then just move over three pixels.
        if (letter == 0)
            drawX = drawX + 3.f;
        else
        {
            // First triangle in quad.
            vertexPtr[index].m_Position = D3DXVECTOR4(drawX, drawY, 0.0f, 1.f);  // Top left.
            vertexPtr[index].m_UV = D3DXVECTOR2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].m_Position = D3DXVECTOR4((drawX + m_Font[letter].size * scaleWidth), (drawY - 16 * scaleHeight), 0.0f, 1.f);  // Bottom right.
            vertexPtr[index].m_UV = D3DXVECTOR2(m_Font[letter].right, 1.0f);
            index++;

            vertexPtr[index].m_Position = D3DXVECTOR4(drawX, (drawY - 16 * scaleHeight), 0.0f, 1.f);  // Bottom left.
            vertexPtr[index].m_UV = D3DXVECTOR2(m_Font[letter].left, 1.0f);
            index++;

            // Second triangle in quad.
            vertexPtr[index].m_Position = D3DXVECTOR4(drawX, drawY, 0.0f, 1.f);  // Top left.
            vertexPtr[index].m_UV = D3DXVECTOR2(m_Font[letter].left, 0.0f);
            index++;

            vertexPtr[index].m_Position = D3DXVECTOR4(drawX + m_Font[letter].size * scaleWidth, drawY, 0.0f, 1.f);  // Top right.
            vertexPtr[index].m_UV = D3DXVECTOR2(m_Font[letter].right, 0.0f);
            index++;

            vertexPtr[index].m_Position = D3DXVECTOR4((drawX + m_Font[letter].size * scaleWidth), (drawY - 16 * scaleHeight), 0.0f, 1.f);  // Bottom right.
            vertexPtr[index].m_UV = D3DXVECTOR2(m_Font[letter].right, 1.0f);
            index++;

            // Update the x location for drawing by the size of the letter and one pixel.
            drawX = drawX + m_Font[letter].size * scaleWidth + LETTER_SPACING;
        }
    }
}
