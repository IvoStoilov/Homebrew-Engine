#pragma once
#include "graphics/isubrenderer.h"
#include "graphics/common/reflectionshader.h"
#include "graphics/common/mesh.h"

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
    void UpdateMoveFactor(f32 dt);

    UniquePtr<Mesh> m_Mesh;
    UniquePtr<ReflectionShader> m_ReflectionShader;
    SharedPtr<Texture> m_DuDvTexture;
    SharedPtr<Texture> m_NormalMap;
    f32 m_WaterLevel = 0.f;
    f32 m_MoveFactor = 0.f;
};