#pragma once
#include "renderer/isubrenderer.h"
#include "renderer/common/texture.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class Terrain;
class ColorShader;
class LightShader;
class TerrainRenderer : public ISubRenderer
{
public:
    TerrainRenderer();

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)   override;
    virtual bool Render(D3D11* d3d)                                                     override;

    virtual void Shutdown()                                                             override;
    
private:
    Terrain* m_Terrain;
    ColorShader* m_WireframeShader;
    LightShader* m_SolidShader;
    SharedPtr<Texture> m_DiffuseTexture;
    SharedPtr<Texture> m_HeightMapTexture;
    SharedPtr<Texture> m_NormalMapTexture;
};