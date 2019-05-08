#pragma once
/*
    Engine is using Left Handd coordinate system

    Using a Half-Ednge Data structure to represent Meshes.
    http://www.sccg.sk/~samuelcik/dgs/half_edge.pdf

    HalfEdge list initialization
    https://stackoverflow.com/questions/15365471/initializing-half-edge-data-structure-from-vertices

    Tangent Computation needed for Normal Mapping
    https://www.marti.works/calculating-tangents-for-your-mesh/
*/
#include "system/math/vec4.h"
#include "system/math/vec2.h"
#include "system/error.h"

struct VertexTypePos
{
    DirectX::XMFLOAT4 m_Position;
    
    void SetValues(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT4& tangent)
    {
        m_Position = pos;
    }
};

struct VertexTypePosUV
{
    DirectX::XMFLOAT4 m_Position;
    DirectX::XMFLOAT2 m_UV;

    void SetValues(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT4& tangent)
    {
        m_Position = pos;
        m_UV = uv;
    }
};

struct VertexTypePosUVNorm
{
    DirectX::XMFLOAT4 m_Position;
    DirectX::XMFLOAT2 m_UV;
    DirectX::XMFLOAT3 m_Normal;

    void SetValues(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT4& tangent)
    {
        m_Position = pos;
        m_UV = uv;
        m_Normal = norm;
    }
};

struct VertexTypePosUVNormTan
{
    DirectX::XMFLOAT4 m_Position;
    DirectX::XMFLOAT2 m_UV;
    DirectX::XMFLOAT3 m_Normal;
    DirectX::XMFLOAT4 m_Tangent;

    void SetValues(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT2& uv, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT4& tangent)
    {
        m_Position = pos;
        m_UV = uv;
        m_Normal = norm;
        m_Tangent = tangent;
    }
};

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
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

        s32 m_SelfIndex;
        s32 m_EndVertexIdx;
        s32 m_PairIdx;
        s32 m_NextIdx;
        s32 m_FaceIdx;

        Edge();
    };

    struct Vertex
    {
        vec4 m_Position;
        vec4 m_Normal;
        vec4 m_Tangent;
        vec2 m_UV;

        Edge* m_Edge;
        s32 m_EdgeIdx;

        Vertex();
        Vertex(const vec4& pos, const vec4& normal, const vec2& uv);
        Vertex(const Vertex& other) = default;
    };

    struct Triangle
    {
        Edge* m_Edge;
        vec4 m_FaceNormal;
        s32 m_EdgeIdx;
        
        Triangle();
    };

public:
    Mesh() = default;
    //should not move or it will break the index lists. or we should rebuld the lists on copy should we need this to happen
    Mesh(const Mesh& other) = delete;
    void operator= (const Mesh& other) = delete;
    
    bool InitializeMeshFromObjFile(const std::string& filepath, bool buildHalfEdgeLis = true);
    void Shutdown();

    void PreSerialize();
    void Serialize(const String& path);
    void Deserialize(const String& path);
    void PostDeserialize();

    //Probably bad, but will rely on external classes(like Model3D or Terrain.h) to upload the data into the GPU
    //Would be nice to optimize data transformations
    //DEPRECATED! SHOULD NOT BE USED.
    inline Array<Vertex>&  GetVertices() { return m_Vertices; }
    inline Array<uint32_t>& GetIndexes()  { return m_Indexes;  }

    void GetAdjacentTriangles(const Vertex& v, Array<Triangle*>& outResult) const;
    void ComputeFaceNormals();

    template<class T>
    bool InitializeBuffers(ID3D11Device* device);
    void Render(ID3D11DeviceContext* deviceContext);
    u32 GetIndexCount() const { return static_cast<u32>(m_Indexes.size()); }

    inline void SetDrawNormals(bool value) { m_DrawNormals = value; }
    void ScaleMesh(f32 scaleFactor);

private:
    void InitializeVertexList(const std::string& filepath);
    void InitializeEdgeListIndexes();
    
    void BuildTangents();
    void BuildHullEdges();
    vec4 ComputeFaceNormal(uint32_t i, uint32_t j, uint32_t k) const;
    vec4 ComputeFaceNormal(const Vertex& a, const Vertex& b, const Vertex& c) const;

    template<class T>
    bool InitializeVertexBuffer(ID3D11Device* device);

    bool InitializeIndexBuffer(ID3D11Device* device);
    void SetupBuffersForWireframe(uint32_t*& outIndexes, uint32_t& outArrSize);
    void SetupBuffersForSolid(uint32_t*& outIndexes, uint32_t& outArrSize);
private:
    u32 m_SizeOfElement = 0;
    bool m_DrawNormals = false;
    Array<Vertex>   m_Vertices;
    Array<Edge>     m_Edges;
    Array<Triangle> m_Triangles;
    Array<uint32_t> m_Indexes;

    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
};

template<class T>
bool Mesh::InitializeBuffers(ID3D11Device* device)
{
    popAssert(InitializeVertexBuffer<T>(device), "");
    popAssert(InitializeIndexBuffer(device), "");
    return true;
}

template<class T>
bool Mesh::InitializeVertexBuffer(ID3D11Device* device)
{
    m_SizeOfElement = sizeof(T);
    T* vertices = new T[m_Vertices.size()];
    memset(vertices, 0, m_Vertices.size() * sizeof(T));

    for (uint32_t i = 0; i < m_Vertices.size(); i++)
    {
        float x = m_Vertices[i].m_Position.x;
        float y = m_Vertices[i].m_Position.y;
        float z = m_Vertices[i].m_Position.z;

        vertices[i].SetValues( DirectX::XMFLOAT4(x, y, z, 0.f)
                             , DirectX::XMFLOAT2(m_Vertices[i].m_UV.x, m_Vertices[i].m_UV.y)
                             , DirectX::XMFLOAT3(m_Vertices[i].m_Normal.x, m_Vertices[i].m_Normal.y, m_Vertices[i].m_Normal.z)
                             , DirectX::XMFLOAT4(m_Vertices[i].m_Tangent.x, m_Vertices[i].m_Tangent.y, m_Vertices[i].m_Tangent.z, m_Vertices[i].m_Tangent.w));

        //if(m_DrawNormals)
        //    g_DebugDisplay->AddLine(m_Vertices[i].m_Position, m_Vertices[i].m_Position + m_Vertices[i].m_Normal, vec4(0.f, 1.f, 0.f, 1.f));
    }

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(T) * m_Vertices.size();
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