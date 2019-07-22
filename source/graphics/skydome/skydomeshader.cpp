#include "precompile.h"
#include "graphics/skydome/skydomeshader.h"

#include "system/error.h"

bool SkydomeShader::InitializeInternal(ID3D11Device* device)
{
    HRESULT result;
    
    D3D11_BUFFER_DESC colorBufferDesc;
    colorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    colorBufferDesc.ByteWidth = sizeof(ColorBufferType);
    colorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    colorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    colorBufferDesc.MiscFlags = 0;
    colorBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&colorBufferDesc, nullptr, &m_ColorBuffer);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateBuffer failed");

    return true;
}

void SkydomeShader::AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd)
{
    polygonLayoutsToAdd.emplace_back(D3D11_INPUT_ELEMENT_DESC());
    D3D11_INPUT_ELEMENT_DESC& polygonLayout = polygonLayoutsToAdd.back();

    polygonLayout.SemanticName = "POSITION";
    polygonLayout.SemanticIndex = 0;
    polygonLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout.InputSlot = 0;
    polygonLayout.AlignedByteOffset = 0;
    polygonLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout.InstanceDataStepRate = 0;
}

void SkydomeShader::ShutdownInternal()
{
    if (m_ColorBuffer)
    {
        m_ColorBuffer->Release();
        m_ColorBuffer = nullptr;
    }
}

bool SkydomeShader::SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams)
{
    const SkydomeShaderParams* skydomeShaderParams = shaderParams.GetAs<SkydomeShaderParams>();
    if (!skydomeShaderParams)
    {
        popAssert(false, "Did not pass SkydomeShaderParams to skydome shader");
        return false;
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_ColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    ColorBufferType* colorDataPtr = (ColorBufferType*)mappedResource.pData;
    colorDataPtr->apexColor = skydomeShaderParams->m_ApexColor;
    colorDataPtr->centerColor = skydomeShaderParams->m_CenterColor;
    deviceContext->Unmap(m_ColorBuffer, 0);
    
    u32 bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_ColorBuffer);

    return true;
}
