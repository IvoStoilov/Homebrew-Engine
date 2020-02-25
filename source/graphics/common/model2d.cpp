#include <graphics/precompile.h>
#include "graphics/common/model2d.h"
#include "graphics/common/texture.h"

#include "system/error.h"
#include "engine/engine.h"
#include "engine/camera.h"

Model2D::Model2D() :
    m_VertexBuffer(nullptr),
    m_IndexBuffer(nullptr),
    m_IndexCount(0),
    m_VertexCount(0),
    m_Texture(nullptr)
{
}

Model2D::~Model2D()
{
}

bool Model2D::Initialize(ID3D11Device* device, uint32_t screenWidth, uint32_t screenHeight, std::string& texturePath, uint32_t modelWidth, uint32_t modelHeight)
{
    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    m_ModelWidth = modelWidth;
    m_ModelHeight = modelHeight;

    m_PreviousePosX = -1;
    m_PreviousePosY = -1;

    bool result = InitializeBuffers(device);
    if (!result)
        return false;

    result = LoadTexture(device, texturePath);
    if (!result)
        return false;

    return true;
}

bool Model2D::InitializeBuffers(ID3D11Device* device)
{
    HRESULT result;
    //TODO (istoilov) Hardcoded crap, again ...

    m_VertexCount = 6;
    m_IndexCount = m_VertexCount;

    VertexType* vertices = new VertexType[m_VertexCount];
    if (vertices == nullptr)
        return false;

    uint32_t* indices = new uint32_t[m_IndexCount];
    if (indices == nullptr)
        return false;

    memset(vertices, 0, (sizeof(VertexType) * m_VertexCount));
    for (uint32_t i = 0; i < m_IndexCount; i++)
    {
        indices[i] = i;
    }

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
    popAssert(!FAILED(result), "Model2D::InitializeBuffers::CreateBuffer failed.");

    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_IndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    popAssert(!FAILED(result), "Model2D::InitializeBuffers::CreateBuffer failed.");

    delete[] vertices;
    delete[] indices;
    
    return true;
}

bool Model2D::LoadTexture(ID3D11Device* device, std::string& filename)
{
    m_Texture = new Texture();
    if (!m_Texture)
    {
        return false;
    }

    // Initialize the texture object.
    bool result = m_Texture->Initialize(device, filename);
    if (!result)
    {
        return false;
    }

    return true;
}

void Model2D::Shutdown()
{
    ReleaseTexture();
    ShutdownBuffers();
}

void Model2D::ReleaseTexture()
{
    // Release the texture object.
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

    return;
}

void Model2D::ShutdownBuffers()
{
    if (m_IndexBuffer)
    {
        m_IndexBuffer->Release();
        m_IndexBuffer = nullptr;
    }

    if (m_VertexBuffer)
    {
        m_VertexBuffer->Release();
        m_VertexBuffer = nullptr;
    }

}

bool Model2D::Render(ID3D11DeviceContext* deviceContext, int32_t posX, int32_t posY)
{
    bool result = UpdateBuffers(deviceContext, posX, posY);
    if (!result)
        return false;

    RenderBuffers(deviceContext);

    return true;
}

bool Model2D::UpdateBuffers(ID3D11DeviceContext* deviceContext, int32_t posX, int32_t posY)
{
    HRESULT result;

    //if (posX == m_PreviousePosX && posY == m_PreviousePosY)
    //    return true;

    m_PreviousePosX = posX;
    m_PreviousePosY = posY;

    float left = ((m_ScreenWidth / 2.f) * -1.f) + (float)posX;
    float right = left + m_ModelWidth;

    float top = (float)((m_ScreenHeight / 2.f) - posY);
    float bottom = top - (float)m_ModelHeight;

    {
        //vec4 modelPos((left + m_ModelWidth / 2.f), (top - m_ModelHeight / 2.f), 0.f, 1.f);
        //vec4 cameraPos = g_Engine->GetCamera()->GetPosition().Tovec4();
        //
        //vec3 zAxis = cameraPos - modelPos;
        //D3DXVec3Normalize(&zAxis, &zAxis);
        //vec3 yAxis = g_Engine->GetCamera()->GetUpAxis().Tovec4();
        //D3DXVec3Normalize(&yAxis, &yAxis);
        //vec3 xAxis;
        //D3DXVec3Cross(&xAxis, &yAxis, &zAxis);

        //m_WorldMatrix._41 = left; //- cameraPosition[0];
        //m_WorldMatrix._42 = right; //- cameraPosition[1];
        //m_WorldMatrix._43 = 0.f;
        //m_WorldMatrix._44 = 1.f;
    }

    VertexType* vertices = new VertexType[m_VertexCount];
    if (vertices == nullptr)
    {
        return false;
    }

    vertices[0].m_Position = vec4(left, top, 0.0f, 0.f);  // Top left.
    vertices[0].m_UV = vec2(0.0f, 0.0f);

    vertices[1].m_Position = vec4(right, bottom, 0.0f, 0.f);  // Bottom right.
    vertices[1].m_UV = vec2(1.0f, 1.0f);

    vertices[2].m_Position = vec4(left, bottom, 0.0f, 0.f);  // Bottom left.
    vertices[2].m_UV = vec2(0.0f, 1.0f);

    // Second triangle.
    vertices[3].m_Position = vec4(left, top, 0.0f, 0.f);  // Top left.
    vertices[3].m_UV = vec2(0.0f, 0.0f);

    vertices[4].m_Position = vec4(right, top, 0.0f, 0.f);  // Top right.
    vertices[4].m_UV = vec2(1.0f, 0.0f);

    vertices[5].m_Position = vec4(right, bottom, 0.0f, 0.f);  // Bottom right.
    vertices[5].m_UV = vec2(1.0f, 1.0f);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    result = deviceContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    popAssert(!FAILED(result), "Model2D::UpdateBuffers::Map failed.");

    VertexType* verticesPtr = (VertexType*)mappedResource.pData;
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_VertexCount));

    deviceContext->Unmap(m_VertexBuffer, 0);

    delete[] vertices;
    
    return true;
}

void Model2D::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    uint32_t stride;
    uint32_t offset;

    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ID3D11ShaderResourceView* Model2D::GetTexture() 
{ 
    return m_Texture->GetTexture(); 
}