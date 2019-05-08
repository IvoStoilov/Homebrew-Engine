#include "precompile.h"
#include "renderer/common/reflectionshader.h"

#include "system/error.h"

bool ReflectionShader::InitializeInternal(ID3D11Device* device)
{
    super::InitializeInternal(device);

    D3D11_BUFFER_DESC reflectionBufferDesc;
    reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
    reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    reflectionBufferDesc.MiscFlags = 0;
    reflectionBufferDesc.StructureByteStride = 0;

    HRESULT result = device->CreateBuffer(&reflectionBufferDesc, nullptr, &m_ReflectionMatrixBuffer);
    popAssert(!FAILED(result), "ColorShader::InitializeShader::CreateBuffer failed");

    return true;
}

bool ReflectionShader::SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams)
{
    super::SetShaderParametersInternal(deviceContext, shaderParams);

    const ReflectionShaderParams* reflectionShaderParams = shaderParams.GetAs<ReflectionShaderParams>();
    if (!reflectionShaderParams)
    {
        popAssert(false, "Did not pass reflectionShaderParams to reflection shader");
        return false;
    }

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_ReflectionMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    popAssert(!FAILED(result), "BaseShader failed to set matrix buffer");

    ReflectionBufferType* dataPtr = reinterpret_cast<ReflectionBufferType*>(mappedResource.pData);
    dataPtr->m_ReflectionMatrix = reflectionShaderParams->m_ReflectionMatrix;
    dataPtr->m_ProjectionMatrix = reflectionShaderParams->m_Projection;

    deviceContext->Unmap(m_ReflectionMatrixBuffer, 0);

    u32 bufferNumber = 2;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_ReflectionMatrixBuffer);

    return true;
}

void ReflectionShader::ShutdownInternal()
{
    if (m_ReflectionMatrixBuffer)
    {
        m_ReflectionMatrixBuffer->Release();
    }
}