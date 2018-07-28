#include "renderer/common/mesh.h"
#include "system/modelloader.h"
#include "system/error.h"

#include <tuple>
#include <map>

/*
    Half Edge List - List Orientation is Counter Clock Wise
*/

typedef std::tuple<int, int> Pair;
typedef std::map< Pair, Mesh::Edge* > EdgeMap;

Mesh::Mesh()
{
}

bool Mesh::InitializeMeshFromFile(const std::string& filepath)
{
    InitializeVertexList(filepath);
    InitializeEdgeList();
    BuildHullEdges();

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

        edgeMap[uv] = new Edge();
        edgeMap[vt] = new Edge();
        edgeMap[tu] = new Edge();

        edgeMap[uv]->m_EndVertex = m_Vertices[std::get<1>(uv)];
        if (m_Vertices[std::get<0>(uv)]->m_Edge == nullptr)
            m_Vertices[std::get<0>(uv)]->m_Edge = edgeMap[uv];

        edgeMap[vt]->m_EndVertex = m_Vertices[std::get<1>(vt)];
        if (m_Vertices[std::get<0>(vt)]->m_Edge == nullptr)
            m_Vertices[std::get<0>(vt)]->m_Edge = edgeMap[vt];

        edgeMap[tu]->m_EndVertex = m_Vertices[std::get<1>(tu)];
        if (m_Vertices[std::get<0>(tu)]->m_Edge == nullptr)
            m_Vertices[std::get<0>(tu)]->m_Edge = edgeMap[tu];

        edgeMap[uv]->m_Next = edgeMap[vt];
        edgeMap[vt]->m_Next = edgeMap[tu];
        edgeMap[tu]->m_Next = edgeMap[uv];

        Triangle* triangle = new Triangle();
        triangle->m_Edge = edgeMap[uv];
        triangle->m_FaceNormal = ComputeFaceNormal(m_Indexes[i], m_Indexes[i + 1], m_Indexes[i + 2]);

        edgeMap[uv]->m_Face = triangle;
        edgeMap[vt]->m_Face = triangle;
        edgeMap[tu]->m_Face = triangle;

        Pair vu = std::make_tuple(m_Indexes[i + 1], m_Indexes[i]);
        Pair tv = std::make_tuple(m_Indexes[i + 2], m_Indexes[i + 1]);
        Pair ut = std::make_tuple(m_Indexes[i], m_Indexes[i + 2]);

        if (edgeMap.find(vu) != edgeMap.end())
        {
            edgeMap[uv]->m_Pair = edgeMap[vu];
            edgeMap[vu]->m_Pair = edgeMap[uv];
        }
        
        if (edgeMap.find(tv) != edgeMap.end())
        {
            edgeMap[vt]->m_Pair = edgeMap[tv];
            edgeMap[tv]->m_Pair = edgeMap[vt];
        }

        if (edgeMap.find(ut) != edgeMap.end())
        {
            edgeMap[tu]->m_Pair = edgeMap[ut];
            edgeMap[ut]->m_Pair = edgeMap[tu];
        }

        m_Edges.push_back(edgeMap[uv]);
        m_Edges.push_back(edgeMap[vt]);
        m_Edges.push_back(edgeMap[tu]);
        m_Triangles.push_back(triangle);
    }
}

void Mesh::Shutdown()
{
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

    vec4 AB = b->m_Position - a->m_Position;
    vec4 AC = c->m_Position - a->m_Position;

    vec4 result = vec4::Cross(AB, AC);
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
        currentEdge->m_EndVertex = hullEdge->m_Next->m_Next->m_EndVertex;
        currentEdge->m_Next = previousEdge;
        currentEdge->m_Pair = hullEdge;
        hullEdge->m_Pair = currentEdge;
        m_Edges.push_back(currentEdge);
        
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