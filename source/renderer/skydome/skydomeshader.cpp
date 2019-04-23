#include "precompile.h"
#include "renderer/skydome/skydomeshader.h"

#include "system/error.h"

bool SkydomeShader::InitializeInternal(ID3D11Device* device, const UniquePtr<ID3D10Blob>& vsBlob, const UniquePtr<ID3D10Blob>& psBlob)
{
    HRESULT result;
    
    D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    u32 numElements; numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vsBlob.get()->GetBufferPointer(),
        vsBlob.get()->GetBufferSize(), &m_Layout);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateInputLayout failed");
  
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
    colorDataPtr->apexColor = skydomeShaderParams->apexColor;
    colorDataPtr->centerColor = skydomeShaderParams->centerColor;
    deviceContext->Unmap(m_ColorBuffer, 0);
    
    u32 bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_ColorBuffer);

    return true;
}
