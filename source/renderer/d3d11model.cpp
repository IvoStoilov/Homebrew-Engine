#include "renderer\d3d11model.h"

std::string CUBE_PATH = "resource/geometry/cube.bgd";

D3D11Model::D3D11Model() :
    m_VertexCount(0),
    m_IndexCount(0),
    m_Texture(nullptr)
{}

D3D11Model::~D3D11Model()
{}

bool D3D11Model::Initialize(ID3D11Device* device, std::string& modelPath, std::string& texturePath)
{
    bool result = ModelLoader::LoadBGDFile(modelPath, m_VertexData);
    if (!result)
        return false;

     result = InitializeBuffers(device);
    if (!result)
        return false;

    result = LoadTexture(device, texturePath);
    if (!result)
        return false;

    return true;
}

void D3D11Model::Shutdown()
{
    ReleaseTexture();
    ShutdownBuffers();
}

void D3D11Model::Render(ID3D11DeviceContext* device)
{
    RenderBuffers(device);
}

bool D3D11Model::InitializeBuffers(ID3D11Device* device)
{
    HRESULT result;

    m_VertexCount = m_VertexData.size();
    m_IndexCount = m_VertexCount; // This should not always be the case;

    VertexType* vertices = new VertexType[m_VertexCount];
    uint32_t* indices = new uint32_t[m_IndexCount];

    for(uint32_t i = 0; i < m_VertexCount; ++i)
    {
        vertices[i].m_Position = D3DXVECTOR4(m_VertexData[i].position[0], m_VertexData[i].position[1], m_VertexData[i].position[2], 1.f);
        vertices[i].m_UV = D3DXVECTOR2(m_VertexData[i].uv[0], m_VertexData[i].position[1]);
        vertices[i].m_Normal = D3DXVECTOR3(m_VertexData[i].normal[0], m_VertexData[i].normal[1], m_VertexData[i].normal[2]);
        indices[i] = i;
    }

    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_BUFFER_DESC indexBufferDesc;

    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_IndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void D3D11Model::ShutdownBuffers()
{
    // Release the index buffer.
    if (m_IndexBuffer)
    {
        m_IndexBuffer->Release();
        m_IndexBuffer = nullptr;
    }

    // Release the vertex buffer.
    if (m_VertexBuffer)
    {     
        m_VertexBuffer->Release();
        m_VertexBuffer = nullptr;
    }
}


bool D3D11Model::LoadTexture(ID3D11Device* device, std::string& texturePath)
{
    bool result = false;

    m_Texture = new Texture();
    if (!m_Texture)
        return false;

    return m_Texture->Initialize(device, texturePath);
}

void D3D11Model::ReleaseTexture()
{
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = nullptr;
    }
}

void D3D11Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}