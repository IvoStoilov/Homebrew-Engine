#include <graphics/precompile.h>
#include "graphics/skydome/skydomerenderer.h"
#include "graphics/skydome/skydomeshader.h"

#include "graphics/d3d11.h"
#include "graphics/common/colorshader.h"
#include "graphics/common/mesh.h"

#include "system/error.h"

#include "engine/engine.h"
#include "engine/camera.h"

std::string SKYDOME_OBJ_PATH = "../../resource/skydome.obj";

// Set the color at the top of the sky dome.
vec4 APEX_COLOR = vec4(0.0f, 0.05f, 0.6f, 1.0f);
vec4 CENTER_COLOR = vec4(0.0f, 0.5f, 0.8f, 1.0f);

bool SkydomeRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_BlueShader = new ColorShader();
    popAssert(m_BlueShader->Initialize(device), "SkydomeRenderer shader failed initing");

    m_SkydomeShader = new SkydomeShader();
    popAssert(m_SkydomeShader->Initialize(device), "SkydomeRenderer shader failed initing");

    m_SkydomeMesh = new Mesh();
    constexpr bool BUILD_HALFEDGE_LIST = false;
    popAssert(m_SkydomeMesh->InitializeMeshFromObjFile(SKYDOME_OBJ_PATH, BUILD_HALFEDGE_LIST), "");
    popAssert(m_SkydomeMesh->InitializeBuffers<VertexTypePos>(device),"");

    return true;
}

bool SkydomeRenderer::Render(D3D11* d3d)
{
    d3d->TurnDepthTestOff();

    const Camera* camera = g_Engine->GetCamera();
    mat4x4 inverseView = m_ViewMatrix.ToXMMATRIX();
    inverseView = mat4x4Inverse(nullptr, inverseView);
    vec4 cameraPos = inverseView.r[3];
    
    m_SkydomeMesh->Render(d3d->GetDeviceContext());

    SkydomeShaderParams skydomeShaderParams;
    skydomeShaderParams.m_World = mat4x4Translation(cameraPos.x, cameraPos.y, cameraPos.z);
    skydomeShaderParams.m_View = m_ViewMatrix.ToXMMATRIX();
    skydomeShaderParams.m_Projection = d3d->GetPerspectiveMatrix();
    skydomeShaderParams.m_ApexColor = APEX_COLOR;
    skydomeShaderParams.m_CenterColor = CENTER_COLOR;
    
    m_SkydomeShader->Render(d3d->GetDeviceContext(), m_SkydomeMesh->GetIndexCount(), skydomeShaderParams);

    d3d->TurnDepthTestOn();

    return true;
}

void SkydomeRenderer::Shutdown()
{
    if (m_SkydomeMesh)
    {
        m_SkydomeMesh->Shutdown();
        delete m_SkydomeMesh;
    }

    if (m_BlueShader)
    {
        m_BlueShader->Shutdown();
        delete m_BlueShader;
    }
}
