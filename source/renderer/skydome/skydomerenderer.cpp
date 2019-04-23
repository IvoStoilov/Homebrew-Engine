#include "precompile.h"
#include "renderer/skydome/skydomerenderer.h"

#include "renderer/skydome/skydomeshader.h"

#include "renderer/d3d11.h"
#include "renderer/common/colorshader.h"
#include "renderer/common/mesh.h"

#include "system/error.h"

#include "core/engine.h"
#include "core/camera.h"

std::string SKYDOME_OBJ_PATH = "../../resource/skydome.obj";

// Set the color at the top of the sky dome.
DirectX::XMFLOAT4 APEX_COLOR = DirectX::XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f);
DirectX::XMFLOAT4 CENTER_COLOR = DirectX::XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f);

bool SkydomeRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_BlueShader = new ColorShader();
    popAssert(m_BlueShader->Initialize(device), "SkydomeRenderer shader failed initing");

    m_SkydomeShader = new SkydomeShader();
    popAssert(m_SkydomeShader->Initialize(device), "SkydomeRenderer shader failed initing");

    m_SkydomeMesh = new Mesh();
    constexpr bool BUILD_HALFEDGE_LIST = false;
    popAssert(m_SkydomeMesh->InitializeMeshFromObjFile(SKYDOME_OBJ_PATH, BUILD_HALFEDGE_LIST), "");
    popAssert(m_SkydomeMesh->InitializeBuffers(device),"");

    return true;
}

bool SkydomeRenderer::Render(D3D11* d3d)
{
    d3d->TurnDepthTestOff();

    const Camera* camera = g_Engine->GetCamera();
    vec4 cameraPos = camera->GetPosition();
    
    D3DXMATRIX projectionMatrix;
    d3d->GetProjectionMatrix(projectionMatrix);

    m_SkydomeMesh->Render(d3d->GetDeviceContext());

    SkydomeShaderParams skydomeShaderParams;
    skydomeShaderParams.world = DirectX::XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z);
    skydomeShaderParams.view = m_ViewMatrix.ToXMMATRIX();
    //todo istoilov: store the projection matrix in XMMATRIX
    skydomeShaderParams.projection = *reinterpret_cast<DirectX::XMMATRIX*>(&projectionMatrix);
    skydomeShaderParams.apexColor = APEX_COLOR;
    skydomeShaderParams.centerColor = CENTER_COLOR;
    
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
