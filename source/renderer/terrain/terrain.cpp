#include "renderer/terrain/terrain.h"
#include "system/error.h"

using namespace DirectX;

//Hardcoded Constants - TODO : make them data-driven
const int32_t DEFAULT_TERRAIN_U_SIZE = 950;
const int32_t DEFAULT_TERRAIN_V_SIZE = 950;

//If true will use DEFAULT_TERRAIN_U_SIZE / DEFAULT_TERRAIN_V_SIZE instead of the height map size
const bool USE_DEFAULT_UV_SIZE = false;

const float TERRAIN_U_RATIO = 1.f;
const float TERRAIN_V_RATIO = 1.f;

const float MAX_TERRAIN_HEIGHT = 100.f;

const XMFLOAT4 DEFAULT_COLOR(1.f, 1.f, 1.f, 1.f);

const std::string TERRAIN_HEIGHT_MAP_PATH = "../../resource/terrain/terrainheightmap3.bmp";

Terrain::Terrain() :
    m_HeightMapUSize(0),
    m_HeightMapVSize(0),
    m_TerrainUSize(DEFAULT_TERRAIN_U_SIZE),
    m_TerrainVSize(DEFAULT_TERRAIN_V_SIZE),
    m_VertexBuffer(nullptr),
    m_IndexBuffer(nullptr),
    m_VertexCount(0),
    m_IndexCount(0)
{}

Terrain::~Terrain()
{}

bool Terrain::Initialize(ID3D11Device* device)
{
    if (!LoadTerrainData())
        return false;

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

bool Terrain::LoadTerrainData()
{
    if (!ModelLoader::LoadBMPFile(TERRAIN_HEIGHT_MAP_PATH, m_HeightMapData, m_HeightMapUSize, m_HeightMapVSize))
        return false;

    if (!USE_DEFAULT_UV_SIZE)
    {
        m_TerrainUSize = m_HeightMapUSize;
        m_TerrainVSize = m_HeightMapVSize;
    }
    
    NormalizeHeight();
    return true;
}

void Terrain::NormalizeHeight()
{
    int32_t ratio = m_HeightMapVSize / m_TerrainVSize;
    for (uint32_t i = 0; i < m_HeightMapData.size(); ++i)
    {
        m_HeightMapData[i].color[0] = (m_HeightMapData[i].color[0] / 255.f) * MAX_TERRAIN_HEIGHT;
        m_HeightMapData[i].uv[0] *= TERRAIN_U_RATIO;
        m_HeightMapData[i].uv[1] *= TERRAIN_V_RATIO;
    }

}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
    // istoilov : The ratio is used for making a normalized mapping of the height map to the real terrain vertexes
    // as it will not be mandatory for the texture pixel size to exactly map the terrain grid
    // BUT! it should be proportional ( investigate if this is a limiting constraint)
    int32_t scaleV = m_HeightMapVSize / m_TerrainVSize;
    int32_t scaleU = m_HeightMapUSize / m_TerrainUSize;

    m_VertexCount = (m_TerrainUSize - 1) * (m_TerrainVSize - 1) * 12;
    m_IndexCount = m_VertexCount;

    VertexType* vertices = new VertexType[m_VertexCount];
    uint32_t* indices = new uint32_t[m_IndexCount];

    uint32_t index = 0;
    for (uint32_t j = 0; j < (m_TerrainVSize - 1); ++j)
    {
        for (uint32_t i = 0; i < (m_TerrainUSize - 1); ++i)
        {
            //Feeding the Texture height. Should not rely on magic numbers and figure out a systemic way to calculate
            //the vertex location in one-dim-array to the grid
            uint32_t index1 = (m_HeightMapVSize * (j + 0) * scaleV) + (i + 0) * scaleU;  // Bottom left.
            uint32_t index2 = (m_HeightMapVSize * (j + 0) * scaleV) + (i + 1) * scaleU;  // Bottom right.
            uint32_t index3 = (m_HeightMapVSize * (j + 1) * scaleV) + (i + 0) * scaleU;  // Upper left.
            uint32_t index4 = (m_HeightMapVSize * (j + 1) * scaleV) + (i + 1) * scaleU;  // Upper right.

            {
                // Upper left.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index3].uv[0], m_HeightMapData[index3].color[0], m_HeightMapData[index3].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;

                // Upper right.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index4].uv[0], m_HeightMapData[index4].color[0], m_HeightMapData[index4].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;
            }

            {
                // Upper right.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index4].uv[0], m_HeightMapData[index4].color[0], m_HeightMapData[index4].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;

                // Bottom left.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index1].uv[0], m_HeightMapData[index1].color[0], m_HeightMapData[index1].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;
            }

            {
                // Bottom left.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index1].uv[0], m_HeightMapData[index1].color[0], m_HeightMapData[index1].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;

                // Upper left.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index3].uv[0], m_HeightMapData[index3].color[0], m_HeightMapData[index3].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;
            }

            {
                // Bottom left.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index1].uv[0], m_HeightMapData[index1].color[0], m_HeightMapData[index1].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;

                // Upper right.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index4].uv[0], m_HeightMapData[index4].color[0], m_HeightMapData[index4].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;
            }

            {
                // Upper right.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index4].uv[0], m_HeightMapData[index4].color[0], m_HeightMapData[index4].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;

                // Bottom right.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index2].uv[0], m_HeightMapData[index2].color[0], m_HeightMapData[index2].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;
            }

            {
                // Bottom right.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index2].uv[0], m_HeightMapData[index2].color[0], m_HeightMapData[index2].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;

                // Bottom left.
                vertices[index].position = XMFLOAT4(m_HeightMapData[index1].uv[0], m_HeightMapData[index1].color[0], m_HeightMapData[index1].uv[1], 0.f);
                vertices[index].color = DEFAULT_COLOR;
                indices[index] = index;
                index++;
            }
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
    uint32_t offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}