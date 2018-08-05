#include "renderer/terrain/terrainrenderer.h"
#include "renderer/terrain/terrain.h"

#include "renderer/common/colorshader.h"
#include "renderer/common/lightshader.h"

#include "renderer/d3d11.h"

#include "system/error.h"
#include "system/commandline/commandlineoptions.h"

D3DXVECTOR4 _DIFFUSE_COLOR(1.f, 1.f, 1.f, 1.f);
D3DXVECTOR4 _LIGHT_DIRECTION(0.577f, -0.577f, 0.577f, 0.f);

bool TerrainRenderer::Render(D3D11* d3d)
{
    D3DXMATRIX worldMatrix;
    D3DXMatrixIdentity(&worldMatrix);
    D3DXMATRIX projectionMatrix;
    d3d->GetProjectionMatrix(projectionMatrix);

    m_Terrain->Render(d3d->GetDeviceContext());
    if (g_CommandLineOptions->m_DrawWireframe)
        m_WireframeShader->Render(d3d->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, m_ViewMatrix.ToD3DXMATRIX(), projectionMatrix);
    else
        m_SolidShader->Render(d3d->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, m_ViewMatrix.ToD3DXMATRIX(), projectionMatrix, nullptr, _DIFFUSE_COLOR, _LIGHT_DIRECTION);

    return true;
}

bool TerrainRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_WireframeShader = new ColorShader();
    popAssert(m_WireframeShader->Initialize(device), "Terrain shader failed initing");

    m_SolidShader = new LightShader();
    popAssert(m_SolidShader->Initialize(device), "Solid Shader failed initing");

    m_Terrain = new Terrain();
    popAssert(m_Terrain->Initialize(device), "Terrain mesh failed initing");

    return true;
}

void TerrainRenderer::Shutdown()
{
    if (m_Terrain)
    {
        m_Terrain->Shutdown();
        delete m_Terrain;
    }

    if (m_WireframeShader)
    {
        m_WireframeShader->Shutdown();
        delete m_WireframeShader;
    }

    if (m_SolidShader)
    {
        m_SolidShader->Shutdown();
        delete m_SolidShader;
    }
}

TerrainRenderer::TerrainRenderer() :
    ISubRenderer(),
    m_ViewMatrix(),
    m_Terrain(nullptr),
    m_WireframeShader(nullptr),
    m_SolidShader(nullptr)
{}

void TerrainRenderer::UpdateViewMatrix(const mat4x4& viewMatrix)
{
    m_ViewMatrix = viewMatrix;
}