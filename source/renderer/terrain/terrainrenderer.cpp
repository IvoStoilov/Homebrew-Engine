#include "precompile.h"
#include "renderer/terrain/terrainrenderer.h"
#include "renderer/terrain/terrain.h"

#include "renderer/common/colorshader.h"
#include "renderer/common/lightshader.h"

#include "renderer/d3d11.h"

#include "system/error.h"
#include "system/commandline/commandlineoptions.h"
#include "system/math/mathutil.h"

DirectX::XMFLOAT4 DIFFUSE_COLOR(1.f, 1.f, 1.f, 1.f);
DirectX::XMFLOAT4 LIGHT_DIRECTION(-.5f, -.2f, .5f, 0.f);
constexpr float AMBIENT_LIGHT_FACTOR = 0.f;
DirectX::XMFLOAT4 AMBIENT_LIGHT(AMBIENT_LIGHT_FACTOR, AMBIENT_LIGHT_FACTOR, AMBIENT_LIGHT_FACTOR, 0.f);

const String HEIGHT_TEXTURE_PATH = "../../resource/terrain/heightMaps/islands2HM.png";
const String NORMAL_TEXTURE_PATH = "../../resource/terrain/islands2/islands2NRML.png";
const String HITE_TEXTURE_PATH   = "../../resource/terrain/islands2/islands2HITE.png";

const String SNOW_ALBEDO_PATH = "../../resource/terrain/albedo/snowAlbedo1k.png";
const String SNOW_HEIGHT_PATH = "../../resource/terrain/albedo/snowHeight1k.png";

const String ROCK_ALBEDO_PATH = "../../resource/terrain/albedo/rockAlbedo1k.png";
const String ROCK_HEIGHT_PATH = "../../resource/terrain/albedo/rockHeight1k.png";

const String GRASS_ALBEDO_PATH = "../../resource/terrain/albedo/grassAlbedo1k.png";

const String SAND_ALBEDO_PATH = "../../resource/terrain/albedo/sandAlbedo1k.png";
const String SAND_HEIGHT_PATH = "../../resource/terrain/albedo/sandHeight1k.png";

const String TERRAIN_VS_PATH = "../../source/renderer/shader/terrainVS.hlsl";
const String TERRAIN_PS_PATH = "../../source/renderer/shader/terrainPS.hlsl";

bool TerrainRenderer::Render(D3D11* d3d)
{
    m_Terrain->Render(d3d->GetDeviceContext());

    if (g_CommandLineOptions->m_DrawWireframe)
    {
        D3DXMATRIX worldMatrix;
        D3DXMatrixIdentity(&worldMatrix);
        D3DXMATRIX projectionMatrix;
        d3d->GetProjectionMatrix(projectionMatrix);
        m_WireframeShader->Render(d3d->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, m_ViewMatrix.ToD3DXMATRIX(), projectionMatrix);
    }
    else
    {
        LightShaderParams params;
        params.m_World = DirectX::XMMatrixIdentity();   //mat4x4::GetRotateXAxisMatrix(MathUtil::ToRads(90)).ToXMMATRIX();
        params.m_View = m_ViewMatrix.ToXMMATRIX();
        params.m_Projection = d3d->GetProjectionMatrix();

        params.m_VSTextures.push_back(m_HeightMapTexture);
        
        params.m_PSTextures.push_back(m_NormalMapTexture);
        params.m_PSTextures.push_back(m_HiteMapTexture);
        params.m_PSTextures.push_back(m_SnowAlbedoTexture);
        params.m_PSTextures.push_back(m_RockAlbedoTexture);
        params.m_PSTextures.push_back(m_GrassAlbedoTexture);
        params.m_PSTextures.push_back(m_SandAlbedoTexture);
        params.m_PSTextures.push_back(m_SandHeightTexture);
        params.m_PSTextures.push_back(m_RockHeightTexture);
        params.m_PSTextures.push_back(m_SnowHeightTexture);

        params.m_DiffuseColor = DIFFUSE_COLOR;
        params.m_LightDirection = LIGHT_DIRECTION;
        params.m_AmbientLight = AMBIENT_LIGHT;
        m_SolidShader->Render(d3d->GetDeviceContext(), m_Terrain->GetIndexCount(), params);
    }

    return true;
}

bool TerrainRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_WireframeShader = new ColorShader();
    popAssert(m_WireframeShader->Initialize(device), "Terrain shader failed initing");

    m_SolidShader = new LightShader(TERRAIN_VS_PATH, TERRAIN_PS_PATH);
    popAssert(m_SolidShader->Initialize(device), "Solid Shader failed initing");

    m_Terrain = new Terrain();
    popAssert(m_Terrain->Initialize(device), "Terrain mesh failed initing");

    m_HeightMapTexture = std::make_shared<Texture>();
    popAssert(m_HeightMapTexture->Initialize(device, HEIGHT_TEXTURE_PATH), "Terrain HEIGHT Texture failed initing");

    m_HiteMapTexture = std::make_shared<Texture>();
    popAssert(m_HiteMapTexture->Initialize(device, HITE_TEXTURE_PATH), "Terrain HITE Texture failed initing");

    m_SnowAlbedoTexture = std::make_shared<Texture>();
    popAssert(m_SnowAlbedoTexture->Initialize(device, SNOW_ALBEDO_PATH), "Terrain SNOW Texture failed initing");

    m_SnowHeightTexture = std::make_shared<Texture>();
    popAssert(m_SnowHeightTexture->Initialize(device, SNOW_HEIGHT_PATH), "Terrain SNOW Texture failed initing");

    m_RockAlbedoTexture = std::make_shared<Texture>();
    popAssert(m_RockAlbedoTexture->Initialize(device, ROCK_ALBEDO_PATH), "Terrain ROCK Texture failed initing");

    m_RockHeightTexture = std::make_shared<Texture>();
    popAssert(m_RockHeightTexture->Initialize(device, ROCK_HEIGHT_PATH), "Terrain ROCK Texture failed initing");

    m_GrassAlbedoTexture = std::make_shared<Texture>();
    popAssert(m_GrassAlbedoTexture->Initialize(device, GRASS_ALBEDO_PATH), "Terrain GRASS Texture failed initing");

    m_SandAlbedoTexture = std::make_shared<Texture>();
    popAssert(m_SandAlbedoTexture->Initialize(device, SAND_ALBEDO_PATH), "Terrain SAND Texture failed initing");

    m_SandHeightTexture = std::make_shared<Texture>();
    popAssert(m_SandHeightTexture->Initialize(device, SAND_HEIGHT_PATH), "Terrain SAND Texture failed initing");

    m_NormalMapTexture = std::make_shared<Texture>();
    popAssert(m_NormalMapTexture->Initialize(device, NORMAL_TEXTURE_PATH), "Terrain Normal Texture failed initing");
    
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
    m_WireframeShader(nullptr),
    m_SolidShader(nullptr)
{}
