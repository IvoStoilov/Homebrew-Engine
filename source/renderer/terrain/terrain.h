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
        DirectX::XMFLOAT2 uv;
        DirectX::XMFLOAT3 normal;
    };

public:
    Terrain();
    ~Terrain();

    bool Initialize(ID3D11Device* device);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext);

    inline const uint32_t GetIndexCount() const { return m_IndexCount; }

private:
    bool InitializeBuffers(ID3D11Device* device);

    bool InitializeTerrainMesh();
    void InitializeTerrainHeight();
    void InitializeTerrainNormals();

    bool LoadTerrainData();
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

    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    uint32_t m_VertexCount;
    uint32_t m_IndexCount;
};