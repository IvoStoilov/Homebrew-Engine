#include "precompile.h"
#include "renderer/common/textureshader.h"
#include "system/error.h"

#include <d3d11.h>

bool TextureShader::InitializeInternal(ID3D11Device* device)
{
    HRESULT result;

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    result = device->CreateSamplerState(&samplerDesc, &m_SampleState);
    popAssert(!FAILED(result), "TextureShader::InitializeShader::CreateSampler failed");

    return true;
}

void TextureShader::AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd)
{
    polygonLayoutsToAdd.emplace_back(D3D11_INPUT_ELEMENT_DESC());
    D3D11_INPUT_ELEMENT_DESC& polygonLayout1 = polygonLayoutsToAdd.back();
    polygonLayout1.SemanticName = "POSITION";
    polygonLayout1.SemanticIndex = 0;
    polygonLayout1.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout1.InputSlot = 0;
    polygonLayout1.AlignedByteOffset = 0;
    polygonLayout1.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout1.InstanceDataStepRate = 0;

    polygonLayoutsToAdd.emplace_back(D3D11_INPUT_ELEMENT_DESC());
    D3D11_INPUT_ELEMENT_DESC& polygonLayout2 = polygonLayoutsToAdd.back();
    polygonLayout2.SemanticName = "TEXCOORD";
    polygonLayout2.SemanticIndex = 0;
    polygonLayout2.Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout2.InputSlot = 0;
    polygonLayout2.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout2.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout2.InstanceDataStepRate = 0;
}

void TextureShader::ShutdownInternal()
{
    if (m_SampleState)
    {
        m_SampleState->Release();
        m_SampleState = 0;
    }
}

bool TextureShader::SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams)
{
    const TextureShaderParams* textureShaderParams = shaderParams.GetAs<TextureShaderParams>();
    if(!textureShaderParams)
    {
        popAssert(false, "Did not pass TextureShaderParams to skydome shader");
        return false;
    }

    Array<ID3D11ShaderResourceView*> resourceViewArray = textureShaderParams->GetShaderResourceViewArray();
    if (!resourceViewArray.empty())
    {
        deviceContext->PSSetShaderResources(0, resourceViewArray.size(), &(resourceViewArray[0]));
    }

    return true;
}