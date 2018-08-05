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

        int32_t m_SelfIndex;
        int32_t m_EndVertexIdx;
        int32_t m_PairIdx;
        int32_t m_NextIdx;
        int32_t m_FaceIdx;

        Edge();
    };

    struct Vertex
    {
        vec4 m_Position;
        vec4 m_Normal;
        vec2 m_UV;

        Edge* m_Edge;
        int32_t m_EdgeIdx;

        Vertex();
        Vertex(const vec4& pos, const vec4& normal, const vec2& uv);
    };

    struct Triangle
    {
        Edge* m_Edge;
        vec4 m_FaceNormal;
        int32_t m_EdgeIdx;
        
        Triangle();
    };

public:
    Mesh();
    
    bool InitializeMeshFromFile(const std::string& filepath);
    void Shutdown();

    void PreSerialize();
    void Serialize(const std::string& path);
    void Deserialize(const std::string& path);
    void PostDeserialize();

    //Probably bad, but will rely on external classes(like Model3D or Terrain.h) to upload the data into the GPU
    //Would be nice to optimize data transformations
    inline std::vector<Vertex*>&  GetVertices() { return m_Vertices; }
    inline std::vector<uint32_t>& GetIndexes()  { return m_Indexes;  }

    void GetAdjacentTriangles(const Vertex* v, std::vector<Triangle*>& outResult) const;
    void ComputeFaceNormals();

private:
    void InitializeVertexList(const std::string& filepath);
    void InitializeEdgeList();
    
    void BuildHullEdges();
    vec4 ComputeFaceNormal(uint32_t i, uint32_t j, uint32_t k);
    vec4 ComputeFaceNormal(Vertex* a, Vertex* b, Vertex* c);

private:
    std::vector<Vertex*>   m_Vertices;
    std::vector<Edge*>     m_Edges;
    std::vector<Triangle*> m_Triangles;
    std::vector<uint32_t> m_Indexes;
};