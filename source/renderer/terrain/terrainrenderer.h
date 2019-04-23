#pragma once
#include "renderer/isubrenderer.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
class D3D11;
class Terrain;
class ColorShader;
class LightShader;
class Texture;
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
    Texture* m_DiffuseTexture;
};