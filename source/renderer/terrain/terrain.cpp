#include "renderer/terrain/terrain.h"
#include "system/error.h"

using namespace DirectX;

//Hardcoded Constants - TODO : make them data-driven
const int32_t TERRAIN_WIDTH  = 256;
const int32_t TERRAIN_HEIGHT = 256;

const XMFLOAT4 DEFAULT_COLOR(1.f, 1.f, 1.f, 1.f);

Terrain::Terrain() :
    m_VertexBuffer(nullptr),
    m_VertexCount(0),
    m_IndexBuffer(nullptr),
    m_IndexCount(0)
{}

Terrain::~Terrain()
{}

bool Terrain::Initialize(ID3D11Device* device)
{
    if (!InitializeBuffers(device))
        return false;

    return true;
}

void Terrain::Shutdown()
{
    ShutdownBuffers();
}

bool Terrain::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);

    return true;
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
    m_VertexCount = (TERRAIN_WIDTH - 1) * (TERRAIN_HEIGHT - 1);
    m_IndexCount = m_VertexCount;

    VertexType* vertices = new VertexType[m_VertexCount];
    uint32_t* indices = new uint32_t[m_IndexCount];

    float positionX;
    float positionZ;
    uint32_t index = 0;
    for (uint32_t j = 0; j < (TERRAIN_HEIGHT - 1); ++j)
    {
        for (uint32_t i = 0; i < (TERRAIN_WIDTH - 1); ++i)
        {
            // Line 1 - Upper left.
            positionX = (float)i;
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 1 - Upper right.
            positionX = (float)(i + 1);
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 2 - Upper right
            positionX = (float)(i + 1);
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 2 - Bottom right.
            positionX = (float)(i + 1);
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 3 - Bottom right.
            positionX = (float)(i + 1);
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 3 - Bottom left.
            positionX = (float)i;
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 4 - Bottom left.
            positionX = (float)i;
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;

            // Line 4 - Upper left.
            positionX = (float)i;
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = DEFAULT_COLOR;
            indices[index] = index;
            index++;
        }
    }

    HRESULT result;

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    popAssert(!FAILED(result), "Terrain Vertex Buffer creation failed");

    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_IndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    popAssert(!FAILED(result), "Terrain Vertex Buffer creation failed");

    delete[] vertices;
    delete[] indices;

    return true;
}


void Terrain::ShutdownBuffers()
{
    if (m_VertexBuffer)
        m_VertexBuffer->Release();

    if (m_IndexBuffer)
        m_IndexBuffer->Release();
}

void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    uint32_t stride = sizeof(VertexType);
    uint32_t offset;

    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}