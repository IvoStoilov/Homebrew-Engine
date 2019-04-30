#include "precompile.h"
#include "renderer/graphicsnode.h"
#include "renderer/common/model3d.h"
#include "renderer/common/lightshader.h"

#include "entitymodel/entity.h"
#include "entitymodel/components/visualcomponent.h"

#include "system/error.h"

//Adding _ to the names because they are global vars and are conflicting during compilation.
//TODO istoilov: make a Light Source object to store such data
DirectX::XMFLOAT4 _DIFFUSE_COLOR(1.f, 1.f, 1.f, 1.f);
DirectX::XMFLOAT4 _LIGHT_DIRECTION(-.3f, +.3f, 1.f, 0.f);

void GraphicsNode::Initialize(ID3D11Device* device)
{
    // Create the model object.
    m_Model = new Model3D();
    popAssert(m_Model != nullptr, "Memory Alloc Failed");
    popAssert(m_Model->Initialize(device, m_Owner->GetModelPath(), m_Owner->GetTexturePath()), "Model Init Failed");

    m_LightShader = new LightShader();
    popAssert(m_LightShader != nullptr, "Memory Alloc Failed");
    popAssert(m_LightShader->Initialize(device), "Shader Init Failed");

    m_TextureShader = new TextureShader();
    popAssert(m_TextureShader != nullptr, "Memory Alloc Failed");
    popAssert(m_TextureShader->Initialize(device), "Shader Init Failed");
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
    Transform globalMatrix = m_Owner->GetOwner()->GetGlobalMatrix();
    
    m_Model->Render(deviceContext);

    //LightShaderParams params;
    //params.m_World = globalMatrix.ToXMMATRIX();
    //params.m_View = m_ViewMatrix.ToXMMATRIX();
    //params.m_Projection = m_ProjectionMatrix.ToXMMATRIX();
    //params.m_PSTextures.push_back(m_Model->GetTexture());
    //params.m_DiffuseColor = _DIFFUSE_COLOR;
    //params.m_LightDirection = _LIGHT_DIRECTION;
    //m_LightShader->Render(deviceContext, m_Model->GetIndexCount(), params);

    TextureShaderParams params;
    params.m_World = globalMatrix.ToXMMATRIX();
    params.m_View = m_ViewMatrix.ToXMMATRIX();
    params.m_Projection = m_ProjectionMatrix.ToXMMATRIX();
    params.m_PSTextures.push_back(m_Model->GetTexture());
    m_TextureShader->Render(deviceContext, m_Model->GetIndexCount(), params);
    
}

GraphicsNode::GraphicsNode(VisualComponent* visComponent) :
    m_Owner(visComponent)
{}

GraphicsNode::~GraphicsNode()
{}