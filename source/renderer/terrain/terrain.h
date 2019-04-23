#pragma once
#include <stdint.h>
#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include "system/modelloader.h"

class Mesh;
class Terrain
{
public:
    Terrain();
    ~Terrain();

    bool Initialize(ID3D11Device* device);
    bool InitializeForBinarize(const std::string& meshOBJFilePath, const std::string& heighMapBMPFilePath);

    void Serialize(const std::string& path);

    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext);

    const uint32_t GetIndexCount() const;
    
    static void BinarizeTerrain(const std::string& meshOBJFilePath, const std::string& heighMapBMPFilePath, const std::string& outputPath);

private:
    bool InitializeTerrainMesh();
    void InitializeTerrainHeight();
    void InitializeTerrainNormals();

    bool LoadHeightMap(const std::string& heightMapPath);
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
};