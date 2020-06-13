#include <graphics/precompile.h>
#include "graphics/terrain/terrainrenderer.h"
#include "graphics/terrain/terrain.h"

#include "graphics/common/colorshader.h"
#include "graphics/common/lightshader.h"

#include "graphics/d3d11.h"

#include "system/error.h"
#include "system/commandline/commandlineoptions.h"
#include "system/math/mathutil.h"

vec4 DIFFUSE_COLOR(1.f, 1.f, 1.f, 1.f);
vec4 LIGHT_DIRECTION(-.5f, -.2f, .5f, 0.f);
constexpr float AMBIENT_LIGHT_FACTOR = 0.f;
vec4 AMBIENT_LIGHT(AMBIENT_LIGHT_FACTOR, AMBIENT_LIGHT_FACTOR, AMBIENT_LIGHT_FACTOR, 0.f);

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

const String TERRAIN_VS_PATH = "../../source/graphics/shader/terrainVS.hlsl";
const String TERRAIN_PS_PATH = "../../source/graphics/shader/terrainPS.hlsl";

bool TerrainRenderer::Render(D3D11* d3d)
{
    popProfile(TerrainRenderer::Render);

    m_Terrain->Render(d3d->GetDeviceContext_DEPRECATED());

    if (g_CommandLineOptions.m_DrawWireframe)
    {
        mat4x4 worldMatrix = mat4x4::Identity;
        mat4x4 projectionMatrix;
        d3d->GetPerspectiveMatrix(projectionMatrix);
        m_WireframeShader->Render(d3d->GetDeviceContext_DEPRECATED(), m_Terrain->GetIndexCount(), worldMatrix, m_ViewMatrix, projectionMatrix);
    }
    else
    {
        LightShaderParams params;
        params.m_World = mat4x4::Identity;
        params.m_View = m_ViewMatrix;
        params.m_Projection = d3d->GetPerspectiveMatrix();

        params.m_VSTextures.push_back(std::make_pair(m_HeightMapTexture  , 0));
        params.m_PSTextures.push_back(std::make_pair(m_NormalMapTexture  , 0));
        params.m_PSTextures.push_back(std::make_pair(m_HiteMapTexture    , 1));
        params.m_PSTextures.push_back(std::make_pair(m_SnowAlbedoTexture , 2));
        params.m_PSTextures.push_back(std::make_pair(m_RockAlbedoTexture , 3));
        params.m_PSTextures.push_back(std::make_pair(m_GrassAlbedoTexture, 4));
        params.m_PSTextures.push_back(std::make_pair(m_SandAlbedoTexture , 5));
        params.m_PSTextures.push_back(std::make_pair(m_SandHeightTexture , 6));
        params.m_PSTextures.push_back(std::make_pair(m_RockHeightTexture , 7));
        params.m_PSTextures.push_back(std::make_pair(m_SnowHeightTexture , 8));

        params.m_ClipPlane = vec4(m_ClipPlane.x, m_ClipPlane.y, m_ClipPlane.z, m_ClipPlane.w);

        params.m_DiffuseColor = DIFFUSE_COLOR;
        params.m_LightDirection = LIGHT_DIRECTION;
        params.m_AmbientLight = AMBIENT_LIGHT;
        m_SolidShader->Render(d3d->GetDeviceContext_DEPRECATED(), m_Terrain->GetIndexCount(), params);
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

    m_HeightMapTexture = std::make_shared<GfxTexture>();
    popAssert(m_HeightMapTexture->Initialize(device, HEIGHT_TEXTURE_PATH), "Terrain HEIGHT Texture failed initing");

    m_HiteMapTexture = std::make_shared<GfxTexture>();
    popAssert(m_HiteMapTexture->Initialize(device, HITE_TEXTURE_PATH), "Terrain HITE Texture failed initing");

    m_SnowAlbedoTexture = std::make_shared<GfxTexture>();
    popAssert(m_SnowAlbedoTexture->Initialize(device, SNOW_ALBEDO_PATH), "Terrain SNOW Texture failed initing");

    m_SnowHeightTexture = std::make_shared<GfxTexture>();
    popAssert(m_SnowHeightTexture->Initialize(device, SNOW_HEIGHT_PATH), "Terrain SNOW Texture failed initing");

    m_RockAlbedoTexture = std::make_shared<GfxTexture>();
    popAssert(m_RockAlbedoTexture->Initialize(device, ROCK_ALBEDO_PATH), "Terrain ROCK Texture failed initing");

    m_RockHeightTexture = std::make_shared<GfxTexture>();
    popAssert(m_RockHeightTexture->Initialize(device, ROCK_HEIGHT_PATH), "Terrain ROCK Texture failed initing");

    m_GrassAlbedoTexture = std::make_shared<GfxTexture>();
    popAssert(m_GrassAlbedoTexture->Initialize(device, GRASS_ALBEDO_PATH), "Terrain GRASS Texture failed initing");

    m_SandAlbedoTexture = std::make_shared<GfxTexture>();
    popAssert(m_SandAlbedoTexture->Initialize(device, SAND_ALBEDO_PATH), "Terrain SAND Texture failed initing");

    m_SandHeightTexture = std::make_shared<GfxTexture>();
    popAssert(m_SandHeightTexture->Initialize(device, SAND_HEIGHT_PATH), "Terrain SAND Texture failed initing");

    m_NormalMapTexture = std::make_shared<GfxTexture>();
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
