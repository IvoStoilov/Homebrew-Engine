#pragma once
#include "renderer/isubrenderer.h"
#include "system/math/mat4x4.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class Terrain;
class ColorShader;
class TerrainRenderer : public ISubRenderer
{
public:
    TerrainRenderer();

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)   override;
    virtual bool Render(D3D11* d3d)                                                     override;

    virtual void Shutdown()                                                             override;
    virtual void UpdateViewMatrix(const mat4x4& viewMatrix)                             override;

private:
    mat4x4 m_ViewMatrix;
    Terrain* m_Terrain;
    ColorShader* m_TerrainShader;
};