#include "precompile.h"
#include "renderer/water/waterrenderer.h"

#include "renderer/common/mesh.h"
#include "renderer/common/reflectionshader.h"

#include "renderer/d3d11.h"
#include "renderer/d3d11renderer.h"

#include "system/error.h"

const String QUAD_PATH = "../../resource/terrain/objExports/terrainplane3x3.obj";

bool WaterRenderer::Render(D3D11* d3d)
{
    m_Mesh->Render(d3d->GetDeviceContext());

    ReflectionShaderParams params;
    params.m_World = DirectX::XMMatrixTranslation(0, m_WaterLevel, 0.f);
    params.m_View = m_ViewMatrix.ToXMMATRIX();
    params.m_Projection = d3d->GetProjectionMatrix();

    const SharedPtr<RenderTexture>& renderTextureSPtr = g_RenderEngine->GetReflectionTexture();
    params.m_PSTextures.push_back(std::make_pair(std::static_pointer_cast<Texture>(renderTextureSPtr), 9));

    params.m_ReflectionMatrix = m_ReflectionMatrix.ToXMMATRIX();

    m_ReflectionShader->Render(d3d->GetDeviceContext(), m_Mesh->GetIndexCount(), params);

    return true;
}

bool WaterRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_Mesh = std::make_unique<Mesh>();
    popAssert(m_Mesh->InitializeMeshFromObjFile(QUAD_PATH, false), "Water mesh failed init");
    m_Mesh->ScaleMesh(64.f);
    popAssert(m_Mesh->InitializeBuffers<VertexTypePosUV>(device), "");


    m_ReflectionShader = std::make_unique<ReflectionShader>();
    popAssert(m_ReflectionShader->Initialize(device), "Water Reflected fialed init");

    return true;
}

void WaterRenderer::Shutdown()
{
    if (m_Mesh)
    {
        m_Mesh->Shutdown();
    }

    if (m_ReflectionShader)
    {
        m_ReflectionShader->Shutdown();
    }
}