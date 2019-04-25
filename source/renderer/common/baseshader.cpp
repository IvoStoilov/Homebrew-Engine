#include "precompile.h"
#include "renderer/common/baseshader.h"

#include "system/error.h"

#include <D3DCompiler.h>
#include <d3dx11async.h>

bool BaseShader::Initialize(ID3D11Device* device)
{
    VS_PS_Blobs shaderBlobs = CompileShaders(device, GetVSPath(), GetPSPath());

    InitializeLayout(device, shaderBlobs.first, shaderBlobs.second);
    InitializeMatrixBuffer(device);

    bool result = InitializeInternal(device);

    shaderBlobs.first.get()->Release();
    shaderBlobs.first.release();
    shaderBlobs.second.get()->Release();
    shaderBlobs.second.release();

    return result;
}

BaseShader::VS_PS_Blobs BaseShader::CompileShaders(ID3D11Device* device, const String&& vsPath, const String&& psPath)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;
   
    ID3D10Blob* vertexShaderBuffer = nullptr;

    
#ifdef POP_DEBUG
    u32 shaderCompileFlags = (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS);
#elif POP_RELEASE
    u32 shaderCompileFlags = (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3);
#endif

    result = D3DX11CompileFromFile(vsPath.c_str(), NULL, NULL, "main", "vs_5_0", shaderCompileFlags, 0, NULL,
        &vertexShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "SkydomeShaderVS Compilation Failed.");

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateVertexShader failed");

    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DX11CompileFromFile(psPath.c_str(), NULL, NULL, "main", "ps_5_0", shaderCompileFlags, 0, NULL,
        &pixelShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "Pixel Shader Compilation Failed.");

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateVertexShader failed");

    return std::make_pair(UniquePtr<ID3D10Blob>(vertexShaderBuffer), UniquePtr<ID3D10Blob>(pixelShaderBuffer));
}

void BaseShader::InitializeMatrixBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    HRESULT result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_MatrixBuffer);
    popAssert(!FAILED(result), "ColorShader::InitializeShader::CreateBuffer failed");
}

void BaseShader::InitializeLayout(ID3D11Device* device, const UniquePtr<ID3D10Blob>& vsBlob, const UniquePtr<ID3D10Blob>& psBlob)
{
    Array<D3D11_INPUT_ELEMENT_DESC> polygonLayouts;
    AddPolygonLayout(polygonLayouts);
    HRESULT result = device->CreateInputLayout(&polygonLayouts[0], polygonLayouts.size(), vsBlob.get()->GetBufferPointer(),
        vsBlob.get()->GetBufferSize(), &m_Layout);

    popAssert(!FAILED(result), "BaseShader::InitializeLayout::CreateInputLayout failed");
}

void BaseShader::Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, const ShaderParamsBase& shaderParams)
{
    SetShaderParameters(deviceContext, shaderParams);
    RenderShader(deviceContext, indexCount);
}

void BaseShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams)
{
    SetMatrixBuffer(deviceContext, shaderParams);
    SetShaderParametersInternal(deviceContext, shaderParams);
}

void BaseShader::SetMatrixBuffer(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    popAssert(!FAILED(result), "BaseShader failed to set matrix buffer");
    
    MatrixBufferType* dataPtr = reinterpret_cast<MatrixBufferType*>(mappedResource.pData);
    dataPtr->world      = XMMatrixTranspose(shaderParams.m_World);
    dataPtr->view       = XMMatrixTranspose(shaderParams.m_View);
    dataPtr->projection = XMMatrixTranspose(shaderParams.m_Projection);

    deviceContext->Unmap(m_MatrixBuffer, 0);

    uint32_t bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
}

void BaseShader::RenderShader(ID3D11DeviceContext* deviceContext, uint32_t indexCount)
{
    deviceContext->IASetInputLayout(m_Layout);

    deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
    deviceContext->PSSetShader(m_PixelShader, nullptr, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void BaseShader::Shutdown()
{
    ShutdownInternal();

    if (m_MatrixBuffer)
    {
        m_MatrixBuffer->Release();
        m_MatrixBuffer = nullptr;
    }

    if (m_Layout)
    {
        m_Layout->Release();
        m_Layout = nullptr;
    }

    if (m_PixelShader)
    {
        m_PixelShader->Release();
        m_PixelShader = nullptr;
    }

    if (m_VertexShader)
    {
        m_VertexShader->Release();
        m_VertexShader = nullptr;
    }
}