#include "precompile.h"
#include "renderer/common/lightshader.h"
#include "system/error.h"

bool LightShader::InitializeInternal(ID3D11Device* device)
{
    super::InitializeInternal(device);

    D3D11_BUFFER_DESC lightBufferDesc;
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    HRESULT result = device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer);
    popAssert(!FAILED(result), "LightShader::InitializeShader::CreateBuffer failed");

    return true;
}

void LightShader::AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd)
{
    super::AddPolygonLayout(polygonLayoutsToAdd);

    polygonLayoutsToAdd.emplace_back(D3D11_INPUT_ELEMENT_DESC());
    D3D11_INPUT_ELEMENT_DESC& polygonLayout = polygonLayoutsToAdd.back();
    polygonLayout.SemanticName = "NORMAL";
    polygonLayout.SemanticIndex = 0;
    polygonLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout.InputSlot = 0;
    polygonLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout.InstanceDataStepRate = 0;
}

void LightShader::ShutdownInternal()
{
    super::ShutdownInternal();

    if (m_LightBuffer)
    {
        m_LightBuffer->Release();
        m_LightBuffer = nullptr;
    }
}

bool LightShader::SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams)
{
    super::SetShaderParametersInternal(deviceContext, shaderParams);

    const LightShaderParams* lightShaderParams = shaderParams.GetAs<LightShaderParams>();
    if (!lightShaderParams)
    {
        popAssert(false, "Did not pass SkydomeShaderParams to skydome shader");
        return false;
    }
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    popAssert(!FAILED(result), "LightShader::SetShaderParametersInternal::Light Mapping failed.");

    LightBufferType* lightData = reinterpret_cast<LightBufferType*>(mappedResource.pData);
    lightData->m_DiffuseColor   = lightShaderParams->m_DiffuseColor;
    lightData->m_LightDirection = lightShaderParams->m_LightDirection;
    lightData->m_AmbientLight   = lightShaderParams->m_AmbientLight;
    
    deviceContext->Unmap(m_LightBuffer, 0);
    lightData = nullptr;

    // need to start assigning slot numbers to constnt buffers 
    // re-read https://gamedev.stackexchange.com/questions/86818/how-does-direct3d-know-if-a-constant-buffer-is-for-the-vertex-or-pixel-shader
    u32 bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightBuffer);
    
    return true;
}

LightShader::LightShader(const String& vsPath, const String& psPath) :
    super(),
    m_VSPath(vsPath),
    m_PSPath(psPath)
{}

LightShader::LightShader() :
    super(),
    m_VSPath("../../source/renderer/shader/diffuselightingVS.hlsl"),
    m_PSPath("../../source/renderer/shader/diffuselightingPS.hlsl")
{}