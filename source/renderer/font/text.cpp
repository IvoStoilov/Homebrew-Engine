#include "renderer/font/text.h"
#include "renderer/font/font.h"
#include "renderer/font/fontshader.h"

const std::string FONT_TEXTURE_PATH = "../../resource/font/fontPic.png";
const std::string FONT_DATA_PATH = "../../resource/font/fontData.txt";

const std::string VS_SHADER_PATH = "../../source/renderer/shader/fontVS.hlsl";
const std::string PS_SHADER_PATH = "../../source/renderer/shader/fontPS.hlsl";

Text::Text() :
    m_Font(nullptr),
    m_FontShader(nullptr),
    m_Text1(nullptr),
    m_Text2(nullptr)
{}

Text::~Text()
{}

bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint16_t screenWidth, uint16_t screenHeight, D3DXMATRIX baseViewMatrix)
{
    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    m_BaseViewMatrix = baseViewMatrix;

    m_Font = new Font();
    if (!m_Font->Initialize(device, FONT_DATA_PATH, FONT_TEXTURE_PATH))
        return false;

    m_FontShader = new FontShader();
    if (!m_FontShader->InitializeShader(device, VS_SHADER_PATH, PS_SHADER_PATH))
        return false;

    if (!InitializeSentence(&m_Text1, 16, device))
        return false;
    
    if (!UpdateSentence(m_Text1, "Pishi Kur", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext))
        return false;

    if (!InitializeSentence(&m_Text2, 16, device))
        return false;

    if (!UpdateSentence(m_Text1, "i da begame", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext))
        return false;

    return true;
}

void Text::Shutdown()
{
    // Release the first sentence.
    ReleaseSentence(&m_Text1);

    // Release the second sentence.
    ReleaseSentence(&m_Text2);

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
    result = RenderSentence(deviceContext, m_Text1, worldMatrix, orthoMatrix);
    if (!result)
    {
        return false;
    }

    // Draw the second sentence.
    result = RenderSentence(deviceContext, m_Text2, worldMatrix, orthoMatrix);
    if (!result)
    {
        return false;
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