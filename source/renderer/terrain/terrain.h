#pragma once
#include <stdint.h>
#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include "system/modelloader.h"

class Mesh;
class Terrain
{
private:
    struct VertexType
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 uv;
    };

public:
    Terrain();
    ~Terrain();

    bool Initialize(ID3D11Device* device);
    bool InitializeForBinarize(const std::string& meshOBJFilePath, const std::string& heighMapBMPFilePath);

    void Serialize(const std::string& path);

    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext);

    inline const uint32_t GetIndexCount() const { return m_IndexCount; }
    

    static void BinarizeTerrain(const std::string& meshOBJFilePath, const std::string& heighMapBMPFilePath, const std::string& outputPath);

private:
    bool InitializeVertexBuffer(ID3D11Device* device);
    bool InitializeIndexBuffer(ID3D11Device* device);

    bool InitializeTerrainParams();
    bool InitializeTerrainMesh();
    void InitializeTerrainHeight();
    void InitializeTerrainNormals();

    bool LoadHeightMap(const std::string& heightMapPath);
    void NormalizeHeight();
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* deviceContext);


    void SetupBuffersForWireframe(uint32_t*& indexes);
    void SetupBuffersForSolid    (uint32_t*& indexes);
    float TestHeightInUV(float x, float y);

private:
    std::vector<ModelLoader::VertexData> m_HeightMapData;

    Mesh* m_Mesh;

    int32_t m_HeightMapUSize;
    int32_t m_HeightMapVSize;

    int32_t m_TerrainUSize;
    int32_t m_TerrainVSize;

    float m_ScaleU;
    float m_ScaleV;
    float m_MaxTerrainHeight;
    std::string m_TerrainHeightMapPath;
    std::string m_TerrainOBJPath;

    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    uint32_t m_VertexCount;
    uint32_t m_IndexCount;
};