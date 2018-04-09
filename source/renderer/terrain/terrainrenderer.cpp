#include "renderer/terrain/terrainrenderer.h"
#include "renderer/terrain/terrain.h"

#include "renderer/common/colorshader.h"
#include "renderer/d3d11.h"

#include "system/error.h"

bool TerrainRenderer::Render(D3D11* d3d)
{
    D3DXMATRIX worldMatrix;
    D3DXMatrixIdentity(&worldMatrix);
    D3DXMATRIX projectionMatrix;
    d3d->GetProjectionMatrix(projectionMatrix);

    m_Terrain->Render(d3d->GetDeviceContext());
    m_TerrainShader->Render(d3d->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, m_ViewMatrix.ToD3DXMATRIX(), projectionMatrix);

    return true;
}

bool TerrainRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_TerrainShader = new ColorShader();
    popAssert(m_TerrainShader->Initialize(device), "Terrain shader failed initing");

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

    if (m_TerrainShader)
    {
        m_TerrainShader->Shutdown();
        delete m_TerrainShader;
    }
}

TerrainRenderer::TerrainRenderer() :
    ISubRenderer(),
    m_ViewMatrix(),
    m_Terrain(nullptr),
    m_TerrainShader(nullptr)
{}

void TerrainRenderer::UpdateViewMatrix(const mat4x4& viewMatrix)
{
    m_ViewMatrix = viewMatrix;
}