#pragma once
/*
    Using a Half-Ednge Data structure to represent Meshes.
    http://www.sccg.sk/~samuelcik/dgs/half_edge.pdf

    HalfEdge list initialization
    https://stackoverflow.com/questions/15365471/initializing-half-edge-data-structure-from-vertices

    TODO (istoilov) : Consider Serialization and prebacking data format to reduce load times.
*/
#include "system/math/vec4.h"
#include "system/math/vec2.h"
#include <vector>
#include <string.h>

class Mesh
{
public:
    struct Vertex;
    struct Triangle;
    struct Edge
    {
        Vertex*   m_EndVertex;
        Edge*     m_Pair;
        Edge*     m_Next;
        Triangle* m_Face;
    };

    struct Vertex
    {
        vec4 m_Position;
        vec4 m_Normal;
        vec2 m_UV;

        Edge* m_Edge;

        Vertex(const vec4& pos, const vec4& normal, const vec2& uv) :
            m_Position(pos),
            m_Normal(normal),
            m_UV(uv)
        {}
    };

    struct Triangle
    {
        Edge* m_Edge;
    };
public:
    Mesh();
    
    bool InitializeMeshFromFile(const std::string& filepath);
    void Shutdown();

    //Probably bad, but will rely on external classes(like Model3D or Terrain.h) to upload the data into the GPU
    //Would be nice to optimize data transformations
    inline std::vector<Vertex*>&  GetVertices() { return m_Vertices; }
    inline std::vector<uint32_t>& GetIndexes()  { return m_Indexes;  }

private:
    void InitializeVertexList(const std::string& filepath);
    void InitializeEdgeList();

private:
    std::vector<Vertex*>   m_Vertices;
    std::vector<Edge*>     m_Edges;
    std::vector<Triangle*> m_Triangles;
    std::vector<uint32_t> m_Indexes;
};