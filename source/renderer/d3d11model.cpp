#include "renderer\d3d11model.h"

D3D11Model::D3D11Model() :
    m_VertexCount(0),
    m_IndexCount(0)
{}

D3D11Model::~D3D11Model()
{}

bool D3D11Model::Initialize(ID3D11Device* device)
{
    return InitializeBuffers(device);
}

void D3D11Model::Shutdown()
{
    ShutdownBuffers();
}

void D3D11Model::Render(ID3D11DeviceContext* device)
{
    RenderBuffers(device);
}

bool D3D11Model::InitializeBuffers(ID3D11Device* device)
{
    HRESULT result;

    VertexType* vertices = nullptr;
    uint32_t* indices = nullptr;

    //Hardcoded shit:
    {
        m_VertexCount = 3;
        m_IndexCount = 3;

        vertices = new VertexType[3];
        indices = new uint32_t[3];

        // Load the vertex array with data.
        vertices[0].m_Position = D3DXVECTOR4(1.0f, -1.0f, 0.0f, 1.f);  // Bottom right.
        vertices[0].m_Color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

        vertices[1].m_Position = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.f);  // Top middle.
        vertices[1].m_Color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

        vertices[2].m_Position = D3DXVECTOR4(-1.0f, -1.0f, 0.0f, 1.f);  // Bottom left.
        vertices[2].m_Color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

        // Load the index array with data.
        indices[0] = 0;  // Bottom left.
        indices[1] = 1;  // Top middle.
        indices[2] = 2;  // Bottom right.
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