#include "renderer/graphicsnode.h"
#include "renderer/d3d11model.h"
#include "renderer/lightshader.h"

#include "entitymodel/entity.h"
#include "system/math/mat4x4.h"
#include "system/error.h"

//should be cleaned
#include <d3d11.h>
#include <d3dx10math.h>

D3DXVECTOR4 DIFFUSE_COLOR(1.f, 1.f, 1.f, 1.f);
D3DXVECTOR4 LIGHT_DIRECTION(-.3f, +.3f, 1.f, 0.f);

/*
"resource/ubisoft-logo.png";
"resource/ink-splatter-texture.png";
"resource/metal_texture.jpg";
*/
std::string TEXTURE_PATH = "../../resource/metal_texture.jpg";
std::string MODEL_PATH = "../../resource/geometry/cube.bgd";

void GraphicsNode::Initialize(ID3D11Device* device)
{
    // Create the model object.
    m_Model = new D3D11Model();
    popAssert(m_Model != nullptr, "Memory Alloc Failed");
    popAssert(m_Model->Initialize(device, MODEL_PATH, TEXTURE_PATH), "Model Init Failed");

    m_LightShader = new LightShader();
    popAssert(m_LightShader != nullptr, "Memory Alloc Failed");
    popAssert(m_LightShader->Initialize(device), "Shader Init Failed");
}

void GraphicsNode::Shutdown()
{
   if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
    }
    
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
    }
}

void GraphicsNode::Render(ID3D11DeviceContext* deviceContext)
{
    Transform globalMatrix = m_Owner->GetGlobalMatrix();
    D3DXMATRIX worldMatrix = globalMatrix.ToD3DXMATRIX(); // Should make a copy or it will get fucked up in Render ( we transpose inside)

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model->Render(deviceContext);

    // Render the model using the color shader.
    //m_Shader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    //m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
    m_LightShader->Render(deviceContext, m_Model->GetIndexCount(), worldMatrix, m_ViewMatrix.ToD3DXMATRIX(), m_ProjectionMatrix.ToD3DXMATRIX(), m_Model->GetTexture(), DIFFUSE_COLOR, LIGHT_DIRECTION);
}

GraphicsNode::GraphicsNode(Entity* entity) :
    m_Owner(entity)
{}

GraphicsNode::~GraphicsNode()
{}