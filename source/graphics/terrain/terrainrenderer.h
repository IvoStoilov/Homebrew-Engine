#pragma once
#include "graphics/isubrenderer.h"
#include "graphics/common/texture.h"

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

    SharedPtr<GfxTexture> m_HeightMapTexture;
    SharedPtr<GfxTexture> m_NormalMapTexture;
    SharedPtr<GfxTexture> m_HiteMapTexture;

    SharedPtr<GfxTexture> m_SnowAlbedoTexture;
    SharedPtr<GfxTexture> m_SnowHeightTexture;

    SharedPtr<GfxTexture> m_RockAlbedoTexture;
    SharedPtr<GfxTexture> m_RockHeightTexture;

    SharedPtr<GfxTexture> m_SandHeightTexture;
    SharedPtr<GfxTexture> m_SandAlbedoTexture;

    SharedPtr<GfxTexture> m_GrassAlbedoTexture;
};