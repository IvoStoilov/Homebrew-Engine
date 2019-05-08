#pragma once
#include "renderer/isubrenderer.h"
#include "renderer/common/reflectionshader.h"
#include "renderer/common/mesh.h"

class WaterRenderer : public ISubRenderer
{
public:
    WaterRenderer(f32 waterLevel) :
        m_WaterLevel(waterLevel)
    {}

    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)   override;
    virtual bool Render(D3D11* d3d)                                                     override;

    virtual void Shutdown()                                                             override;

private:
    UniquePtr<Mesh> m_Mesh;
    UniquePtr<ReflectionShader> m_ReflectionShader;
    f32 m_WaterLevel;
};