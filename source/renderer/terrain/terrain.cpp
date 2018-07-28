#include "renderer/terrain/terrain.h"
#include "renderer/common/mesh.h"
#include "system/error.h"

using namespace DirectX;

//Hardcoded Constants - TODO : make them data-driven
const int32_t DEFAULT_TERRAIN_U_SIZE = 256;
const int32_t DEFAULT_TERRAIN_V_SIZE = 256;

//If true will use DEFAULT_TERRAIN_U_SIZE / DEFAULT_TERRAIN_V_SIZE instead of the height map size
const bool USE_DEFAULT_UV_SIZE = true;
const bool DRAW_WIREFRAME = false;

const float TERRAIN_U_RATIO = 1.f;
const float TERRAIN_V_RATIO = 1.f;

const float MAX_TERRAIN_HEIGHT = 60.f;

const XMFLOAT4 DEFAULT_COLOR(1.f, 1.f, 1.f, 1.f);
const XMFLOAT4 DEEP_COLOR(0.2f, 1.f, 0.2f, 1.f);
const XMFLOAT4 GRAY_COLOR(0.5f, 0.5f, 0.5f, 1.f);


const std::string TERRAIN_HEIGHT_MAP_PATH = "../../resource/terrain/terrainheightmap3.bmp";
const std::string TERRAIN_MESH_PATH = "../../resource/terrain/terrainplane256x256.obj";
//const std::string TERRAIN_MESH_PATH = "../../resource/terrain/3x3plane.obj";

const std::string TERRAIN_MESH_BIN = "../../resource/terrain/TerrainMesh.bin";

Terrain::Terrain() :
    m_Mesh(nullptr),
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

    if (!InitializeTerrainMesh())
        return false;

    if (!InitializeBuffers(device))
        return false;

    return true;
}

void Terrain::Shutdown()
{
    m_Mesh->Shutdown();
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

bool Terrain::InitializeTerrainMesh()
{
    m_Mesh = new Mesh();
    //{
    //    m_Mesh->InitializeMeshFromFile(TERRAIN_MESH_PATH);
    //    InitializeTerrainHeight();
    //    m_Mesh->ComputeFaceNormals();
    //    InitializeTerrainNormals();
    //}
    
    m_Mesh->Deserialize(TERRAIN_MESH_BIN);

    return true;
}

void Terrain::InitializeTerrainHeight()
{
    std::vector<Mesh::Vertex*>& meshVertices = m_Mesh->GetVertices();
    for (uint32_t i = 0; i < meshVertices.size(); ++i)
    {
        float height = TestHeightInUV(meshVertices[i]->m_Position.x, meshVertices[i]->m_Position.z);
        meshVertices[i]->m_Position.y = height;
    }
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

XMFLOAT4 ColorSelector(float height)
{
    return GRAY_COLOR;
    return height < 40 ? DEEP_COLOR : DEFAULT_COLOR;
}

void Terrain::InitializeTerrainNormals()
{
    std::vector<Mesh::Vertex*>& vertices = m_Mesh->GetVertices();

    for (Mesh::Vertex* v : vertices)
    {
        std::vector<Mesh::Triangle*> adjFaces;
        m_Mesh->GetAdjacentTriangles(v, adjFaces);
        vec4 normal = vec4::Zero;
        for (Mesh::Triangle* face : adjFaces)
        {
            normal += face->m_FaceNormal;
        }

        if (adjFaces.size() != 0)
            normal /= adjFaces.size();
        else
            normal = vec4::BaseJ;

        v->m_Normal = normal;
    }
}

void Terrain::SetupBuffersForSolid(uint32_t*& indexes)
{
    std::vector<uint32_t>& meshIndexes = m_Mesh->GetIndexes();
    indexes = new uint32_t[m_IndexCount];

    for (uint32_t i = 0; i < m_IndexCount; i++)
    {
        indexes[i] = meshIndexes[i];
    }
}

void Terrain::SetupBuffersForWireframe(uint32_t*& indexes)
{
    std::vector<uint32_t>& meshIndexes = m_Mesh->GetIndexes();
    m_IndexCount *= 2;
    indexes = new uint32_t[m_IndexCount];

    uint32_t j = 0;
    for (uint32_t i = 0; i < meshIndexes.size(); i+=3)
    {
        indexes[j++] = meshIndexes[i];
        indexes[j++] = meshIndexes[i + 1];
        indexes[j++] = meshIndexes[i + 1];
        indexes[j++] = meshIndexes[i + 2];
        indexes[j++] = meshIndexes[i + 2];
        indexes[j++] = meshIndexes[i];
    }
}

float Terrain::TestHeightInUV(float x, float y)
{
    float normX = (x / DEFAULT_TERRAIN_U_SIZE) + 0.5f;
    float normY = (y / DEFAULT_TERRAIN_V_SIZE) + 0.5f;

    uint32_t heightMapX = (uint32_t)(normX * (m_HeightMapUSize -1));
    uint32_t heightMapY = (uint32_t)(normY * (m_HeightMapVSize -1));

    return m_HeightMapData[heightMapX * m_HeightMapUSize + heightMapY].color[0];
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
    // istoilov : The ratio is used for making a normalized mapping of the height map to the real terrain vertexes
    // as it will not be mandatory for the texture pixel size to exactly map the terrain grid
    // BUT! it should be proportional ( investigate if this is a limiting constraint)
    int32_t scaleV = m_HeightMapVSize / m_TerrainVSize;
    int32_t scaleU = m_HeightMapUSize / m_TerrainUSize;

    std::vector<Mesh::Vertex*>& meshVertices = m_Mesh->GetVertices();
    std::vector<uint32_t>& meshIndexes = m_Mesh->GetIndexes();

    m_VertexCount = meshVertices.size();
    m_IndexCount = meshIndexes.size();
    
    VertexType* vertices = nullptr;
    uint32_t*   indices  = nullptr;

    vertices = new VertexType[m_VertexCount];
    for (uint32_t i = 0; i < m_VertexCount; i++)
    {
        vertices[i].position = XMFLOAT4(meshVertices[i]->m_Position.x * scaleU, meshVertices[i]->m_Position.y, meshVertices[i]->m_Position.z * scaleV, 0.f);
        vertices[i].uv = XMFLOAT2(meshVertices[i]->m_UV.x, meshVertices[i]->m_UV.y);
        vertices[i].normal = XMFLOAT3(meshVertices[i]->m_Normal.x, meshVertices[i]->m_Normal.y, meshVertices[i]->m_Normal.z);
        //vertices[i].color = ColorSelector(vertices[i].position.y);
    }

    if (DRAW_WIREFRAME)
        SetupBuffersForWireframe(indices);
    else
        SetupBuffersForSolid(indices);


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
    D3D_PRIMITIVE_TOPOLOGY drawTopology = DRAW_WIREFRAME ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    deviceContext->IASetPrimitiveTopology(drawTopology);
}