#include <graphics/precompile.h>
#include "graphics/water/waterrenderer.h"

#include "graphics/common/mesh.h"
#include "graphics/common/reflectionshader.h"

#include "graphics/d3d11.h"
#include "graphics/d3d11renderer.h"

#include "system/error.h"

const String QUAD_PATH = "../../resource/terrain/objExports/terrainplane3x3.obj";
const String DUDV_WATER_PATH = "../../resource/terrain/islands2/waterDUDV.png";
const String NORMAL_MAP_PATH = "../../resource/terrain/islands2/waterNRML.png";

constexpr f32 WAVE_SPEED = 0.03f;

bool WaterRenderer::Render(D3D11* d3d)
{
    m_Mesh->Render(d3d->GetDeviceContext());

    ReflectionShaderParams params;
    params.m_World = mat4x4::CreateTranslation(0, m_WaterLevel, 0.f);
    params.m_View = m_ViewMatrix;
    params.m_Projection = d3d->GetProjectionMatrix();

    const SharedPtr<RenderTexture>& renderTextureSPtr = g_RenderEngine->GetReflectionTexture();
    const SharedPtr<RenderTexture>& refractionTexture = g_RenderEngine->GetRefractionTexture();
    params.m_PSTextures.push_back(std::make_pair(std::static_pointer_cast<Texture>(renderTextureSPtr), 9));
    params.m_PSTextures.push_back(std::make_pair(std::static_pointer_cast<Texture>(refractionTexture), 10));
    params.m_PSTextures.push_back(std::make_pair(m_DuDvTexture, 11));
    params.m_PSTextures.push_back(std::make_pair(m_NormalMap, 12));

    params.m_ReflectionMatrix = m_ReflectionMatrix;

    UpdateMoveFactor(g_RenderEngine->GetDT());
    params.m_MoveFactor = m_MoveFactor;

    m_ReflectionShader->Render(d3d->GetDeviceContext(), m_Mesh->GetIndexCount(), params);

    return true;
}

void WaterRenderer::UpdateMoveFactor(f32 dt)
{
    m_MoveFactor += (WAVE_SPEED * dt);
    m_MoveFactor = fmod(m_MoveFactor, 1.f);
}


bool WaterRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_Mesh = std::make_unique<Mesh>();
    popAssert(m_Mesh->InitializeMeshFromObjFile(QUAD_PATH, false), "Water mesh failed init");
    m_Mesh->ScaleMesh(64.f);
    popAssert(m_Mesh->InitializeBuffers<VertexTypePosUV>(device), "");

    m_ReflectionShader = std::make_unique<ReflectionShader>();
    popAssert(m_ReflectionShader->Initialize(device), "Water Reflected fialed init");

    m_DuDvTexture = std::make_shared<Texture>();
    popAssert(m_DuDvTexture->Initialize(device, DUDV_WATER_PATH), "Water DuDv faild init");

    m_NormalMap = std::make_shared<Texture>();
    popAssert(m_NormalMap->Initialize(device, NORMAL_MAP_PATH), "Water Normals faild init");

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