#include "renderer/common/mesh.h"
#include "system/modelloader.h"
#include "system/error.h"
#include <map>
#include <tuple>

Mesh::Mesh()
{
}

bool Mesh::InitializeMeshFromFile(const std::string& filepath)
{
    InitializeVertexList(filepath);
    InitializeEdgeList();

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
    typedef std::tuple<int, int> Pair;
    typedef std::map<Pair, Edge*> EdgeMap;
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
        edgeMap[vt]->m_EndVertex = m_Vertices[std::get<1>(vt)];
        edgeMap[tu]->m_EndVertex = m_Vertices[std::get<1>(tu)];

        edgeMap[uv]->m_Next = edgeMap[vt];
        edgeMap[vt]->m_Next = edgeMap[tu];
        edgeMap[tu]->m_Next = edgeMap[uv];

        Triangle* triangle = new Triangle();
        triangle->m_Edge = edgeMap[uv];

        edgeMap[uv]->m_Face = triangle;
        edgeMap[vt]->m_Face = triangle;
        edgeMap[tu]->m_Face = triangle;

        Pair vu = std::make_tuple(m_Indexes[i + 1], m_Indexes[i]);
        Pair tv = std::make_tuple(m_Indexes[i + 2], m_Indexes[i + 1]);
        Pair ut = std::make_tuple(m_Indexes[i + 1], m_Indexes[i + 2]);

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