#include <graphics/precompile.h>
#include "graphics/debugdisplay/debugdisplayrenderer.h"

#include "graphics/common/colorshader.h"
#include "graphics/d3d11.h"

#include "system/error.h"

bool DebugDisplayRenderer::Render(D3D11* d3d)
{
    mat4x4 worldMatrix;
    mat4x4 Identity(worldMatrix);
    mat4x4 projectionMatrix;
    d3d->GetProjectionMatrix(projectionMatrix);

    RenderGeometry(d3d->GetDeviceContext());
    m_Shader->Render(d3d->GetDeviceContext(), m_Elements.size() * 2 , worldMatrix, m_ViewMatrix, projectionMatrix);

    return true;
}

bool DebugDisplayRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    InitializeBuffers(device);

    m_Shader = new ColorShader();
    popAssert(m_Shader->Initialize(device), "DebugDisplay shader failed initing");

    return true;
}

void DebugDisplayRenderer::InitializeBuffers(ID3D11Device* device)
{
    if (m_Elements.empty())
        return;

    uint32_t vertexDataSize = m_Elements.size() * 2;
    VertexType* vertexData = new VertexType[vertexDataSize];
    uint32_t*   indices = new uint32_t[vertexDataSize];
    for (uint32_t i = 0; i < m_Elements.size(); ++i)
    {
        vertexData[2 * i].m_Position = m_Elements[i].m_Start;
        vertexData[2 * i].m_Color = m_Elements[i].m_Color;
        vertexData[2 * i + 1].m_Position = m_Elements[i].m_End;
        vertexData[2 * i + 1].m_Color = m_Elements[i].m_Color;
        indices[2 * i] = 2 * i;
        indices[2 * i + 1] = 2 * i + 1;
    }

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexDataSize;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexDataDesc;
    vertexDataDesc.pSysMem = vertexData;
    vertexDataDesc.SysMemPitch = 0;
    vertexDataDesc.SysMemSlicePitch = 0;

    HRESULT result;
    result = device->CreateBuffer(&vertexBufferDesc, &vertexDataDesc, &m_VertexBuffer);
    popAssert(!FAILED(result), "Terrain Vertex Buffer creation failed");

    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * vertexDataSize;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
    popAssert(!FAILED(result), "Terrain Vertex Buffer creation failed");

    delete[] vertexData;
    delete[] indices;
}

void DebugDisplayRenderer::RenderGeometry(ID3D11DeviceContext* deviceContext)
{
    uint32_t stride = sizeof(VertexType);
    uint32_t offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    D3D_PRIMITIVE_TOPOLOGY drawTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    deviceContext->IASetPrimitiveTopology(drawTopology);
}

void DebugDisplayRenderer::Shutdown()
{
    if (m_VertexBuffer)
        m_VertexBuffer->Release();

    if (m_IndexBuffer)
        m_IndexBuffer->Release();

    if (m_Shader)
    {
        m_Shader->Shutdown();
        delete m_Shader;
    }
}

void DebugDisplayRenderer::AddLine(const vec4& start, const vec4& end, const vec4 color)
{
    DebugDisplayElement element;
    element.m_Start = start;
    element.m_End = end;
    element.m_Color = color;
    m_Elements.push_back(element);
}