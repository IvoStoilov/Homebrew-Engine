#include "renderer/terrain/terrain.h"
#include "renderer/d3d11renderer.h"
#include "renderer/common/mesh.h"
#include "renderer/debugdisplay/DebugDisplayRenderer.h"
#include "system/error.h"
#include "system/profiling/profilemanager.h"
#include "system/commandline/commandlineoptions.h"

using namespace DirectX;

//Hardcoded Constants - used as a fall back
constexpr float DEFAULT_SCALE_U = .5f;
constexpr float DEFAULT_SCALE_V = .5f;
constexpr float MAX_TERRAIN_HEIGHT = 50.f;
//Temp Hack for the Half size. Need to figure out how to store meta data for the mesh or probrably for the terrain
constexpr float TERRAIN_HALF_SIZE = 128.f; 

const std::string TERRAIN_MESH_BIN = "../../resource/terrain/bin/terrain513x513.bin";
  

Terrain::Terrain() :
    m_Mesh(nullptr),
    m_HeightMapUSize(0),
    m_HeightMapVSize(0),
    m_TerrainUSize(0),
    m_TerrainVSize(0),
    m_ScaleU(1.f),
    m_ScaleV(1.f),
    m_MaxTerrainHeight(25.f),
    m_TerrainHeightMapPath(""),
    m_TerrainOBJPath(""),
    m_VertexBuffer(nullptr),
    m_IndexBuffer(nullptr),
    m_VertexCount(0),
    m_IndexCount(0)
{}

Terrain::~Terrain()
{
}

bool Terrain::Initialize(ID3D11Device* device)
{
    PROFILE_FUNCTION(Terrain::Initialize);

    if (!InitializeTerrainParams())
        return false;

    if (!InitializeTerrainMesh())
        return false;

    if (!InitializeVertexBuffer(device))
        return false;

    if (!InitializeIndexBuffer(device))
        return false;

    return true;
}

bool Terrain::InitializeTerrainParams()
{
    //TOOD : investigate 3rd party lib for parsing XMLs - RapidJSON?
    m_ScaleU = DEFAULT_SCALE_U;
    m_ScaleV = DEFAULT_SCALE_V;
    m_MaxTerrainHeight = MAX_TERRAIN_HEIGHT;

    return true;
}

bool Terrain::InitializeTerrainMesh()
{
    m_Mesh = new Mesh();
    m_Mesh->Deserialize(TERRAIN_MESH_BIN);
    return true;
}

bool Terrain::InitializeVertexBuffer(ID3D11Device* device)
{
    std::vector<Mesh::Vertex*>& meshVertices = m_Mesh->GetVertices();
    m_VertexCount = (uint32_t)meshVertices.size();

    VertexType* vertices = new VertexType[m_VertexCount];
    for (uint32_t i = 0; i < m_VertexCount; i++)
    {
        float x = meshVertices[i]->m_Position.x * m_ScaleU;
        float y = meshVertices[i]->m_Position.y / 255.f * m_MaxTerrainHeight;
        float z = meshVertices[i]->m_Position.z * m_ScaleV;

        vertices[i].position = XMFLOAT4(x, y, z, 0.f);
        vertices[i].normal = XMFLOAT3(meshVertices[i]->m_Normal.x, meshVertices[i]->m_Normal.y, meshVertices[i]->m_Normal.z);
        vertices[i].uv = XMFLOAT2(meshVertices[i]->m_UV.x, meshVertices[i]->m_UV.y);
        g_DebugDisplay->AddLine(meshVertices[i]->m_Position, (meshVertices[i]->m_Position + meshVertices[i]->m_Normal * 2), vec4(0.f, 1.f, 0.f, 1.f));
    }

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

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    popAssert(!FAILED(result), "Terrain Vertex Buffer creation failed");

    delete[] vertices;
    return !FAILED(result);
}

bool Terrain::InitializeIndexBuffer(ID3D11Device* device)
{
    std::vector<uint32_t>& meshIndexes = m_Mesh->GetIndexes();
    m_IndexCount = (uint32_t)meshIndexes.size();
    uint32_t* indices = nullptr;

    if (g_CommandLineOptions->m_DrawWireframe)
        SetupBuffersForWireframe(indices);
    else
        SetupBuffersForSolid(indices);

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

    HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    popAssert(!FAILED(result), "Terrain Vertex Buffer creation failed");
    
    delete[] indices;
    return !FAILED(result);
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
    for (uint32_t i = 0; i < meshIndexes.size(); i += 3)
    {
        indexes[j++] = meshIndexes[i];
        indexes[j++] = meshIndexes[i + 1];
        indexes[j++] = meshIndexes[i + 1];
        indexes[j++] = meshIndexes[i + 2];
        indexes[j++] = meshIndexes[i + 2];
        indexes[j++] = meshIndexes[i];
    }
}

void Terrain::Shutdown()
{
    m_Mesh->Shutdown();
    delete m_Mesh;

    ShutdownBuffers();
}

bool Terrain::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);

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
    D3D_PRIMITIVE_TOPOLOGY drawTopology = g_CommandLineOptions->m_DrawWireframe ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    deviceContext->IASetPrimitiveTopology(drawTopology);
}

#pragma region BINARIZE
void Terrain::BinarizeTerrain(const std::string& meshOBJFilePath, const std::string& heighMapBMPFilePath, const std::string& outputPath)
{
    PROFILE_FUNCTION(Terrain::BinarizeTerrain)

    Terrain* terrain = new Terrain();
    if (terrain->InitializeForBinarize(meshOBJFilePath, heighMapBMPFilePath))
        terrain->Serialize(outputPath);

    terrain->Shutdown();
    delete terrain;
}

void Terrain::Serialize(const std::string& path)
{
    m_Mesh->Serialize(path);
}

bool Terrain::InitializeForBinarize(const std::string& meshOBJFilePath, const std::string& heighMapBMPFilePath)
{
    m_Mesh = new Mesh();
    bool result = true;

    result = LoadHeightMap(heighMapBMPFilePath);
    popAssert(result, "Terrain::InitializeForBinarize::LoadTerrainData failed");

    result = m_Mesh->InitializeMeshFromFile(meshOBJFilePath);
    popAssert(result, "Terrain::InitializeForBinarize::InitializeMeshFromFile failed");

    InitializeTerrainHeight();

    m_Mesh->ComputeFaceNormals();
    InitializeTerrainNormals();

    return true;
}

bool Terrain::LoadHeightMap(const std::string& path)
{
    if (!ModelLoader::LoadBMPFile(path, m_HeightMapData, m_HeightMapUSize, m_HeightMapVSize))
        return false;

    m_TerrainUSize = m_HeightMapUSize;
    m_TerrainVSize = m_HeightMapVSize;

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
            normal /= (float)adjFaces.size();
        else
            normal = vec4::BaseJ;

        v->m_Normal = normal;
    }
}

float Terrain::TestHeightInUV(float x, float y)
{
    uint32_t heightMapX = (uint32_t)(x + TERRAIN_HALF_SIZE);
    uint32_t heightMapY = (uint32_t)(y + TERRAIN_HALF_SIZE);

    return m_HeightMapData[heightMapX * m_HeightMapUSize + heightMapY].color[0];
}
#pragma endregion