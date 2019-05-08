#include "precompile.h"
#include "renderer/terrain/terrain.h"
#include "renderer/d3d11renderer.h"
#include "renderer/common/mesh.h"
#include "renderer/debugdisplay/DebugDisplayRenderer.h"

#include "system/error.h"
#include "system/profiling/profilemanager.h"
#include "system/commandline/commandlineoptions.h"

//Temp Hack for the Half size. Need to figure out how to store meta data for the mesh or probrably for the terrain
constexpr f32 TERRAIN_HALF_SIZE = 128.f; 
constexpr f32 TERRAIN_SCALE = 128.f;

const std::string TERRAIN_MESH_BIN = "../../resource/terrain/bin/islands2PosUVNormYUp.bin";
  

Terrain::Terrain() :
    m_Mesh(nullptr),
    m_HeightMapUSize(0),
    m_HeightMapVSize(0),
    m_TerrainUSize(0),
    m_TerrainVSize(0),
    m_ScaleU(1.f),
    m_ScaleV(1.f),
    m_MaxTerrainHeight(25.f)
{}

bool Terrain::Initialize(ID3D11Device* device)
{
    PROFILE_FUNCTION(Terrain::Initialize);

    if (!InitializeTerrainMesh())
        return false;

    if (!m_Mesh->InitializeBuffers<VertexTypePosUVNormTan>(device))
        return false;

    return true;
}

bool Terrain::InitializeTerrainMesh()
{
    m_Mesh = new Mesh();
    m_Mesh->Deserialize(TERRAIN_MESH_BIN);
    //m_Mesh->SetDrawNormals(true);
    m_Mesh->ScaleMesh(TERRAIN_SCALE);
    return true;
}

void Terrain::Shutdown()
{
    m_Mesh->Shutdown();
    delete m_Mesh;
}

bool Terrain::Render(ID3D11DeviceContext* deviceContext)
{
    m_Mesh->Render(deviceContext);
    return true;
}

const uint32_t Terrain::GetIndexCount() const
{ 
    return m_Mesh->GetIndexes().size(); 
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
    bool result = false;
    if (!meshOBJFilePath.empty())
    {
        result = m_Mesh->InitializeMeshFromObjFile(meshOBJFilePath, false);
        popAssert(result, "Terrain::InitializeForBinarize::InitializeMeshFromObjFile failed");
    }

    if (!heighMapBMPFilePath.empty())
    {
        result = LoadHeightMap(heighMapBMPFilePath);
        popAssert(result, "Terrain::InitializeForBinarize::LoadTerrainData failed");

        InitializeTerrainHeight();

        m_Mesh->ComputeFaceNormals();
        InitializeTerrainNormals();
    }

    return result;
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
    std::vector<Mesh::Vertex>& meshVertices = m_Mesh->GetVertices();
    for (uint32_t i = 0; i < meshVertices.size(); ++i)
    {
        float height = TestHeightInUV(meshVertices[i].m_Position.x, meshVertices[i].m_Position.z);
        meshVertices[i].m_Position.y = height;
    }
}

void Terrain::InitializeTerrainNormals()
{
    std::vector<Mesh::Vertex>& vertices = m_Mesh->GetVertices();

    for (Mesh::Vertex& v : vertices)
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

        v.m_Normal = normal;
    }
}

float Terrain::TestHeightInUV(float x, float y)
{
    uint32_t heightMapX = (uint32_t)(x + TERRAIN_HALF_SIZE);
    uint32_t heightMapY = (uint32_t)(y + TERRAIN_HALF_SIZE);

    heightMapX *= 2;
    heightMapY *= 2;

    return m_HeightMapData[heightMapX * m_HeightMapUSize + heightMapY].color[0] / 3.25f;
}
#pragma endregion