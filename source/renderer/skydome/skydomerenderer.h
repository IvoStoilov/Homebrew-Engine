#pragma once
#include "renderer/isubrenderer.h"

class Mesh;
class ColorShader;
class SkydomeShader;
class D3D11;
struct ID3D11Device;
struct ID3D11DeviceContext;
class SkydomeRenderer : public ISubRenderer
{
public:
    SkydomeRenderer() = default;

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)   override;
    virtual bool Render(D3D11* d3d)                                                     override;

    virtual void Shutdown()                                                             override;

private:
    Mesh* m_SkydomeMesh = nullptr;
    ColorShader* m_BlueShader = nullptr;
    SkydomeShader* m_SkydomeShader = nullptr;
};