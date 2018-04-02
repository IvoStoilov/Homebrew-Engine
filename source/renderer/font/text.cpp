#include "renderer/font/text.h"
#include "renderer/font/font.h"
#include "renderer/font/fontshader.h"

const std::string FONT_TEXTURE_PATH = "../../resource/font/fontPic.png";
const std::string FONT_DATA_PATH = "../../resource/font/fontData.txt";

const std::string VS_SHADER_PATH = "../../source/renderer/shader/fontVS.hlsl";
const std::string PS_SHADER_PATH = "../../source/renderer/shader/fontPS.hlsl";

const uint32_t LINE_SPACEING = 16;

Text::Text() :
    m_Font(nullptr),
    m_FontShader(nullptr),
    m_Lines(nullptr)
{}

Text::~Text()
{}

bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint32_t maxLength, uint16_t screenWidth, uint16_t screenHeight, uint16_t numberOfLines, int16_t posX, int16_t posY)
{
    if (!deviceContext)
        return false;

    m_NumberOfLines = numberOfLines;
    m_Lines = new SentenceType*[m_NumberOfLines];
    if (!m_Lines)
        return false;

    m_DeviceContextCache = deviceContext;

    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    m_Font = new Font();
    if (!m_Font->Initialize(device, FONT_DATA_PATH, FONT_TEXTURE_PATH))
        return false;

    m_FontShader = new FontShader();
    if (!m_FontShader->Initialize(device))
        return false;

    for (uint32_t i = 0; i < m_NumberOfLines; ++i)
    {
        m_Lines[i] = new SentenceType();
        if (!InitializeSentence(&m_Lines[i], maxLength, device))
            return false;

        if (!UpdateSentence(m_Lines[i], "Lorem Ipsum", posX, posY - (i * LINE_SPACEING), 1.0f, 0.f, 0.0f))
            return false;
    }
    
    return true;
}

void Text::Shutdown()
{
    for (uint16_t i = 0; i < m_NumberOfLines; ++i)
        ReleaseSentence(&m_Lines[i]);
   
    delete[] m_Lines;

    // Release the font shader object.
    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
    }

    // Release the font object.
    if (m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
    }
}

bool Text::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
    bool result;

    // Draw the first sentence.
    for (uint16_t i = 0; i < m_NumberOfLines; ++i)
    {
        result = RenderSentence(deviceContext, m_Lines[i], worldMatrix, orthoMatrix);
        if (!result)
        {
            return false;
        }
    }

    return true;
}

bool Text::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;


    // Create a new sentence object.
    *sentence = new SentenceType;
    if (!*sentence)
    {
        return false;
    }

    // Initialize the sentence buffers to null.
    (*sentence)->vertexBuffer = 0;
    (*sentence)->indexBuffer = 0;

    // Set the maximum length of the sentence.
    (*sentence)->maxLength = maxLength;

    // Set the number of vertices in the vertex array.
    (*sentence)->vertexCount = 6 * maxLength;

    // Set the number of indexes in the index array.
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // Create the vertex array.
    vertices = new VertexType[(*sentence)->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new unsigned long[(*sentence)->indexCount];
    if (!indices)
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // Initialize the index array.
    for (i = 0; i < (*sentence)->indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Release the vertex array as it is no longer needed.
    delete[] vertices;
    vertices = 0;

    // Release the index array as it is no longer needed.
    delete[] indices;
    indices = 0;

    return true;
}

bool Text::UpdateSentence(SentenceType* sentence, char* text, int posX, int posY, float r, float g, float b)
{
    sentence->red = r;
    sentence->green = g;
    sentence->blue = b;

    sentence->posX = posX;
    sentence->posY = posY;

    int32_t numLetters = strlen(text);
    if (numLetters > sentence->maxLength)
        return false;

    VertexType* vertices = new VertexType[sentence->vertexCount];
    memset(vertices, 0, sizeof(VertexType) * sentence->vertexCount);

    float drawX = (float)(m_ScreenWidth/2.f * -1 + posX);
    float drawY = (float)(m_ScreenHeight / 2.f * -1 + posY);

    m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = m_DeviceContextCache->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    memcpy((VertexType*)mappedResource.pData, vertices, sizeof(VertexType) * sentence->vertexCount);

    m_DeviceContextCache->Unmap(sentence->vertexBuffer, 0);
    delete[] vertices;

    return true;
}

bool Text::SetText(char* text, uint16_t slot)
{
    int32_t numLetters = strlen(text);
    if (numLetters > m_Lines[slot]->maxLength)
        return false;

    return UpdateSentence(m_Lines[slot], text, m_Lines[slot]->posX, m_Lines[slot]->posY, m_Lines[slot]->red, m_Lines[slot]->green, m_Lines[slot]->blue);
}

void Text::ReleaseSentence(SentenceType** sentence)
{
    if (*sentence)
    {
        if ((*sentence)->vertexBuffer)
            (*sentence)->vertexBuffer->Release();

        if ((*sentence)->indexBuffer)
            (*sentence)->indexBuffer->Release();

        delete *sentence;
    }
}

bool Text::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
    unsigned int stride, offset;
    D3DXVECTOR4 pixelColor;
    bool result;

    stride = sizeof(VertexType);
    offset = 0;

    m_DeviceContextCache->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
    m_DeviceContextCache->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_DeviceContextCache->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

    D3DXMATRIX id;
    D3DXMatrixIdentity(&id);

    if(m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, id, orthoMatrix, m_Font->GetTexture(), pixelColor))
    {
        false;
    }

    return true;
}