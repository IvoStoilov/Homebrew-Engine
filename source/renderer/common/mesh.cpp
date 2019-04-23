#include "renderer/common/mesh.h"
#include "system/modelloader.h"
#include "system/error.h"
#include "system/profiling/profilemanager.h"
#include "system/commandline/commandlineoptions.h"

#include <tuple>
#include <map>
#include <fstream>
#include <d3d11.h>
#include <directxmath.h>

typedef std::tuple<int, int> Pair;
typedef std::map< Pair, Mesh::Edge* > EdgeMap;

constexpr bool REBUILD_HALFEDGELIST_ENABLED = false;

Mesh::Mesh()
{
}

bool Mesh::InitializeMeshFromObjFile(const std::string& filepath, bool buildHalfEdgeList/*=true*/)
{
    InitializeVertexList(filepath);
    if (buildHalfEdgeList)
    {
        InitializeEdgeList();
        BuildHullEdges();
    }

    return true;
}

void Mesh::InitializeVertexList(const std::string& filepath)
{
    m_Vertices.clear();
    m_Triangles.clear();
    m_Indexes.clear();

    typedef std::tuple<uint32_t, uint32_t, uint32_t> Key;
    typedef std::tuple<Mesh::Vertex*, std::vector<uint32_t> >Value;
    typedef std::map<Key, Value> VertexMap;
    VertexMap vertexMap;

    RawGeometryData rawData;
    if (ModelLoader::LoadOBJFile(filepath, rawData))
    {
        uint32_t indexListSize = rawData.m_PosIndexes.size();
        for (uint32_t i = 0; i < indexListSize; i++)
        {
            Key current = std::make_tuple(rawData.m_PosIndexes[i], rawData.m_NormIndexes[i], rawData.m_UVIndexes[i]);
            if (vertexMap.find(current) != vertexMap.end())
            {
                Value& v = vertexMap[current];
                std::vector<uint32_t>& indexList = std::get<1>(v);
                indexList.push_back(i);
            }
            else
            {
                Mesh::Vertex* vertex = new Vertex(vec4(rawData.m_Positions[rawData.m_PosIndexes[i]]),
                                                  vec4(rawData.m_Normals[rawData.m_NormIndexes[i]]),
                                                  vec2(rawData.m_UVs[rawData.m_UVIndexes[i]]));
                
                std::vector<uint32_t> indexList;
                indexList.push_back(i);
                Value v = std::make_tuple(vertex, indexList);
                vertexMap[current] = v;
            }
        }

        m_Indexes.resize(indexListSize);
        for (VertexMap::iterator it = vertexMap.begin(); it != vertexMap.end(); ++it)
        {
            Value& v = it->second;
            m_Vertices.push_back(std::get<0>(v));

            std::vector<uint32_t>& indexList = std::get<1>(v);
            for (uint32_t i : indexList)
            {
                m_Indexes[i] = m_Vertices.size() - 1;
            }
        }
    }
}

void Mesh::InitializeEdgeList()
{
    m_Edges.clear();
    EdgeMap edgeMap;
    
    popAssert(m_Indexes.size() != 0, "Index size is 0, Probably not read from file.");

    for (uint32_t i = 0; i < m_Indexes.size() - 2; i += 3)
    {
        Pair uv = std::make_tuple(m_Indexes[i], m_Indexes[i + 1]);
        Pair vt = std::make_tuple(m_Indexes[i + 1], m_Indexes[i + 2]);
        Pair tu = std::make_tuple(m_Indexes[i + 2], m_Indexes[i]);

        Edge* edgeUV = new Edge();
        Edge* edgeVT = new Edge();
        Edge* edgeTU = new Edge();

        edgeMap[uv] = edgeUV;
        edgeMap[vt] = edgeVT;
        edgeMap[tu] = edgeTU;

        edgeUV->m_SelfIndex = m_Edges.size();
        m_Edges.push_back(edgeUV);
        edgeVT->m_SelfIndex = m_Edges.size();
        m_Edges.push_back(edgeVT);
        edgeTU->m_SelfIndex = m_Edges.size();
        m_Edges.push_back(edgeTU);

        edgeUV->m_EndVertex    = m_Vertices[std::get<1>(uv)];
        edgeUV->m_EndVertexIdx = std::get<1>(uv);
        if (m_Vertices[std::get<0>(uv)]->m_Edge == nullptr)
        {
            m_Vertices[std::get<0>(uv)]->m_Edge    = edgeUV;
            m_Vertices[std::get<0>(uv)]->m_EdgeIdx = edgeUV->m_SelfIndex;
        }

        edgeVT->m_EndVertex    = m_Vertices[std::get<1>(vt)];
        edgeVT->m_EndVertexIdx = std::get<1>(vt);
        if (m_Vertices[std::get<0>(vt)]->m_Edge == nullptr)
        {
            m_Vertices[std::get<0>(vt)]->m_Edge    = edgeVT;
            m_Vertices[std::get<0>(vt)]->m_EdgeIdx = edgeVT->m_SelfIndex;
        }

        edgeTU->m_EndVertex    = m_Vertices[std::get<1>(tu)];
        edgeTU->m_EndVertexIdx = std::get<1>(tu);
        if (m_Vertices[std::get<0>(tu)]->m_Edge == nullptr)
        {
            m_Vertices[std::get<0>(tu)]->m_Edge = edgeTU;
            m_Vertices[std::get<0>(tu)]->m_EdgeIdx = edgeTU->m_SelfIndex;
        }

        edgeUV->m_Next    = edgeVT;
        edgeUV->m_NextIdx = edgeVT->m_SelfIndex;

        edgeVT->m_Next    = edgeTU;
        edgeVT->m_NextIdx = edgeTU->m_SelfIndex;

        edgeTU->m_Next    = edgeUV;
        edgeTU->m_NextIdx = edgeUV->m_SelfIndex;

        Triangle* triangle = new Triangle();
        uint32_t triangleIdx = m_Triangles.size();
        m_Triangles.push_back(triangle);
        triangle->m_Edge    = edgeUV;
        triangle->m_EdgeIdx = edgeUV->m_SelfIndex;
        triangle->m_FaceNormal = ComputeFaceNormal(m_Indexes[i], m_Indexes[i + 1], m_Indexes[i + 2]);

        edgeUV->m_Face = triangle;
        edgeUV->m_FaceIdx = triangleIdx;

        edgeVT->m_Face = triangle;
        edgeVT->m_FaceIdx = triangleIdx;

        edgeTU->m_Face = triangle;
        edgeTU->m_FaceIdx = triangleIdx;

        Pair vu = std::make_tuple(m_Indexes[i + 1], m_Indexes[i]);
        Pair tv = std::make_tuple(m_Indexes[i + 2], m_Indexes[i + 1]);
        Pair ut = std::make_tuple(m_Indexes[i], m_Indexes[i + 2]);

        if (edgeMap.find(vu) != edgeMap.end())
        {
            edgeUV->m_Pair = edgeMap[vu];
            edgeUV->m_PairIdx = edgeMap[vu]->m_SelfIndex;

            edgeMap[vu]->m_Pair = edgeUV;
            edgeMap[vu]->m_PairIdx = edgeUV->m_SelfIndex;
        }
        
        if (edgeMap.find(tv) != edgeMap.end())
        {
            edgeVT->m_Pair = edgeMap[tv];
            edgeVT->m_PairIdx = edgeMap[tv]->m_SelfIndex;

            edgeMap[tv]->m_Pair = edgeVT;
            edgeMap[tv]->m_PairIdx = edgeVT->m_SelfIndex;
        }

        if (edgeMap.find(ut) != edgeMap.end())
        {
            edgeTU->m_Pair = edgeMap[ut];
            edgeTU->m_PairIdx = edgeMap[ut]->m_SelfIndex;

            edgeMap[ut]->m_Pair = edgeTU;
            edgeMap[ut]->m_PairIdx = edgeTU->m_SelfIndex;
        }
    }
}

void Mesh::Shutdown()
{
    if (m_VertexBuffer)
        m_VertexBuffer->Release();

    if (m_IndexBuffer)
        m_IndexBuffer->Release();

    for (Vertex* vertex : m_Vertices)
        delete vertex;

    for (Edge* edge : m_Edges)
        delete edge;

    for (Triangle* face : m_Triangles)
        delete face;
}

vec4 Mesh::ComputeFaceNormal(uint32_t a, uint32_t b, uint32_t c)
{
    return ComputeFaceNormal(m_Vertices[a], m_Vertices[b], m_Vertices[c]);
}

vec4 Mesh::ComputeFaceNormal(Vertex* a, Vertex* b, Vertex* c)
{
    //Using right-hand coordinate system and clockwise face orientation
    vec4 AB = b->m_Position - a->m_Position;
    vec4 AC = c->m_Position - a->m_Position;

    vec4 result = vec4::Cross(AC, AB);
    result.Normalize();
    return result;
}

void Mesh::GetAdjacentTriangles(const Vertex* v, std::vector<Triangle*>& outResult) const
{
    Edge* current = v->m_Edge;
    do
    {
        current = current->m_Pair->m_Next;
        if (current->m_Face)
        {
            outResult.push_back(current->m_Face);
        }
    } while (current != v->m_Edge);
}

void Mesh::BuildHullEdges()
{
    Edge* hullEdge = nullptr;
    for(Edge* edge : m_Edges)
    {
        if (edge->m_Pair == nullptr)
        {
            hullEdge = edge;
            break;
        }
    }

    if (hullEdge == nullptr)
        return; // There are no edges withouth pairs; Nothing to do;

    Edge* previousEdge = nullptr;
    Edge* currentEdge = hullEdge->m_Pair;
    Edge* startEdge = hullEdge;
    while (currentEdge == nullptr)
    {
        currentEdge = new Edge();
        currentEdge->m_SelfIndex = m_Edges.size();
        m_Edges.push_back(currentEdge);

        currentEdge->m_EndVertex = hullEdge->m_Next->m_Next->m_EndVertex;
        for (uint32_t i = 0; i < m_Vertices.size(); ++i)
        {
            if (currentEdge->m_EndVertex == m_Vertices[i])
            {
                currentEdge->m_EndVertexIdx = i;
                break;
            }
        }
        

        currentEdge->m_Next = previousEdge;
        currentEdge->m_NextIdx = (previousEdge) ? previousEdge->m_SelfIndex : -1;

        currentEdge->m_Pair = hullEdge;
        currentEdge->m_PairIdx = hullEdge->m_SelfIndex;

        hullEdge->m_Pair = currentEdge;
        hullEdge->m_PairIdx = currentEdge->m_SelfIndex;
        
        previousEdge = currentEdge;
        
        //step
        Edge* searchEdge = hullEdge->m_EndVertex->m_Edge;
        Vertex* v = hullEdge->m_EndVertex;
        
        do
        {
            if (searchEdge->m_Pair == nullptr)
            {
                currentEdge = searchEdge->m_Pair;
                hullEdge = searchEdge;
            }
            else
            {
                searchEdge = searchEdge->m_Pair->m_Next;
                if (!searchEdge)
                    break;
                currentEdge = searchEdge->m_Pair;
                hullEdge = searchEdge;
            }
        } while (searchEdge != v->m_Edge && currentEdge != nullptr);
    }
    startEdge->m_Pair->m_Next = previousEdge;
    startEdge->m_Pair->m_NextIdx = previousEdge->m_SelfIndex;
}

void Mesh::ComputeFaceNormals()
{
    for (Triangle* face : m_Triangles)
    {
        Edge* faceEdge = face->m_Edge;
        Vertex* a = faceEdge->m_EndVertex;
        Vertex* b = faceEdge->m_Next->m_EndVertex;
        Vertex* c = faceEdge->m_Next->m_Next->m_EndVertex;
        face->m_FaceNormal = ComputeFaceNormal(a, b, c);
    }
}

void Mesh::PreSerialize()
{
    PROFILE_FUNCTION(Mesh::PreSerialize);
}

void Mesh::Serialize(const std::string& path)
{
    PROFILE_FUNCTION(Mesh::Serialize);

    PreSerialize();

    std::ofstream writer;
    writer.open(path, std::ofstream::binary);
    
    uint32_t vertSize = m_Vertices.size();
    writer << vertSize << "\n";

    for (Vertex* vertex : m_Vertices)
    {
        writer << vertex->m_Position.x << " " <<vertex->m_Position.y << " " << vertex->m_Position.z << " ";
        writer << vertex->m_Normal.x   << " " <<vertex->m_Normal.y   << " " << vertex->m_Normal.z   << " ";
        writer << vertex->m_UV.x       << " " <<vertex->m_UV.y << " ";

        writer << vertex->m_EdgeIdx << "\n";
    }

    uint32_t edgeSize = m_Edges.size();
    writer << edgeSize << "\n";
    for (Edge* edge : m_Edges)
    {
        writer << edge->m_EndVertexIdx << " " <<  edge->m_PairIdx << " " << edge->m_NextIdx << " " << edge->m_FaceIdx << " ";
    }

    uint32_t faceSize = m_Triangles.size();
    writer << faceSize << "\n";
    for (Triangle* face : m_Triangles)
    {
        writer << face->m_FaceNormal.x << " " << face->m_FaceNormal.y << " " << face->m_FaceNormal.z << " ";
        writer << face->m_EdgeIdx << "\n";
    }

    uint32_t idxSize = m_Indexes.size();
    writer << idxSize << "\n";
    for (uint32_t idx : m_Indexes)
    {
        writer << idx << " ";
    }

    writer.close();
}

void Mesh::Deserialize(const std::string& path)
{
    PROFILE_FUNCTION(Mesh::Deserialize);

    std::ifstream reader;
    reader.open(path, std::ifstream::binary);

    uint32_t vertSize = 0;
    reader >> vertSize;
    
    for (uint32_t i = 0; i < vertSize; ++i)
    {
        Vertex* vertex = new Vertex();
        reader >> vertex->m_Position.x >> vertex->m_Position.y >> vertex->m_Position.z;
        reader >> vertex->m_Normal.x >> vertex->m_Normal.y >> vertex->m_Normal.z;
        reader >> vertex->m_UV.x >> vertex->m_UV.y;
        
        reader >> vertex->m_EdgeIdx;
        m_Vertices.push_back(vertex);
    }

    uint32_t edgeSize = 0;
    reader >> edgeSize;
    for (uint32_t i = 0; i < edgeSize; ++i)
    {
        Edge* edge = new Edge();
        reader >> edge->m_EndVertexIdx >> edge->m_PairIdx >> edge->m_NextIdx >> edge->m_FaceIdx;
        m_Edges.push_back(edge);
    }

    uint32_t faceSize = 0;
    reader >> faceSize;
    for (uint32_t i = 0; i < faceSize; ++i)
    {
        Triangle* face = new Triangle();
        reader >> face->m_FaceNormal.x >> face->m_FaceNormal.y >> face->m_FaceNormal.z;
        reader >> face->m_EdgeIdx;
        m_Triangles.push_back(face);
    }

    uint32_t idxSize = 0;
    reader >> idxSize;
    for (uint32_t i = 0; i < idxSize; i++)
    {
        uint32_t idx;
        reader >> idx;
        m_Indexes.push_back(idx);
    }

    reader.close();

    PostDeserialize();
}

void Mesh::PostDeserialize()
{
    PROFILE_FUNCTION(Mesh::PostDeserialize);

    if (REBUILD_HALFEDGELIST_ENABLED)
    {
        for (Vertex* vertex : m_Vertices)
        {
            vertex->m_Edge = m_Edges[vertex->m_EdgeIdx];
        }

        for (Edge* edge : m_Edges)
        {
            edge->m_EndVertex = m_Vertices[edge->m_EndVertexIdx];
            edge->m_Pair = m_Edges[edge->m_PairIdx];
            edge->m_Next = m_Edges[edge->m_NextIdx];
            if (edge->m_FaceIdx > 0)
                edge->m_Face = m_Triangles[edge->m_FaceIdx];
        }

        for (Triangle* face : m_Triangles)
        {
            face->m_Edge = m_Edges[face->m_EdgeIdx];
        }
    }
}

struct VertexType
{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;
};

bool Mesh::InitializeVertexBuffer(ID3D11Device* device)
{
    VertexType* vertices = new VertexType[m_Vertices.size()];
    for (uint32_t i = 0; i < m_Vertices.size(); i++)
    {
        float x = m_Vertices[i]->m_Position.x;
        float y = m_Vertices[i]->m_Position.y;
        float z = m_Vertices[i]->m_Position.z;

        vertices[i].position = DirectX::XMFLOAT4(x, y, z, 0.f);
        vertices[i].normal = DirectX::XMFLOAT3(m_Vertices[i]->m_Normal.x, m_Vertices[i]->m_Normal.y, m_Vertices[i]->m_Normal.z);
        vertices[i].uv = DirectX::XMFLOAT2(m_Vertices[i]->m_UV.x, m_Vertices[i]->m_UV.y);
    }

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_Vertices.size();
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

bool Mesh::InitializeIndexBuffer(ID3D11Device* device)
{
    uint32_t* indices = nullptr;
    uint32_t indexCount = 0;

    if (g_CommandLineOptions->m_DrawWireframe)
        SetupBuffersForWireframe(indices, indexCount);
    else
        SetupBuffersForSolid(indices, indexCount);

    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
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

void Mesh::SetupBuffersForWireframe(uint32_t*& indexes, uint32_t& outArrSize)
{
    outArrSize = m_Indexes.size() * 2;
    indexes = new uint32_t[outArrSize];
    uint32_t j = 0;
    for (uint32_t i = 0; i < m_Indexes.size(); i += 3)
    {
        indexes[j++] = m_Indexes[i];
        indexes[j++] = m_Indexes[i + 1];
        indexes[j++] = m_Indexes[i + 1];
        indexes[j++] = m_Indexes[i + 2];
        indexes[j++] = m_Indexes[i + 2];
        indexes[j++] = m_Indexes[i];
    }
}

void Mesh::SetupBuffersForSolid(uint32_t*& indexes, uint32_t& outArrSize)
{
    outArrSize = m_Indexes.size();
    indexes = new uint32_t[outArrSize];

    for (uint32_t i = 0; i < outArrSize; i++)
    {
        indexes[i] = m_Indexes[i];
    }
}

bool Mesh::InitializeBuffers(ID3D11Device* device)
{
    popAssert(InitializeVertexBuffer(device), "");
    popAssert(InitializeIndexBuffer(device), "");
    return true;
}

void Mesh::Render(ID3D11DeviceContext* deviceContext)
{
    uint32_t stride = sizeof(VertexType);
    uint32_t offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    D3D_PRIMITIVE_TOPOLOGY drawTopology = g_CommandLineOptions->m_DrawWireframe ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    deviceContext->IASetPrimitiveTopology(drawTopology);
}

Mesh::Edge::Edge() :
    m_EndVertex(nullptr),
    m_Pair(nullptr),
    m_Next(nullptr),
    m_Face(nullptr),
    m_SelfIndex(-1),
    m_EndVertexIdx(-1),
    m_PairIdx(-1),
    m_NextIdx(-1),
    m_FaceIdx(-1)
{}

Mesh::Vertex::Vertex(const vec4& pos, const vec4& normal, const vec2& uv) :
    m_Position(pos),
    m_Normal(normal),
    m_UV(uv),
    m_Edge(nullptr),
    m_EdgeIdx(-1)
{}

Mesh::Vertex::Vertex() :
    m_Position(vec4::Zero),
    m_Normal(vec4::Zero),
    m_UV(vec2::Zero),
    m_Edge(nullptr),
    m_EdgeIdx(-1)
{}

Mesh::Triangle::Triangle() :
    m_Edge(nullptr),
    m_FaceNormal(vec4::Zero),
    m_EdgeIdx(-1)
{}