#include "renderer/skydome/skydomeshader.h"

#include "system/error.h"

#include <d3d11.h>
#include <D3DCompiler.h>
#include <d3dx11async.h>

const std::string VS_SHADER_PATH = "../../source/renderer/shader/skydomeVS.hlsl";
const std::string PS_SHADER_PATH = "../../source/renderer/shader/skydomePS.hlsl";

bool SkydomeShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device, VS_SHADER_PATH, PS_SHADER_PATH);
}

void SkydomeShader::Shutdown()
{
    ShutdownShader();
}

bool SkydomeShader::InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
    unsigned int numElements;

    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;


    // Compile the vertex shader code.
    result = D3DX11CompileFromFile(vsPath.c_str(), NULL, NULL, "main", "vs_5_0", (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG), 0, NULL,
        &vertexShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "SkydomeShaderVS Compilation Failed.");

    // Compile the pixel shader code.
    result = D3DX11CompileFromFile(psPath.c_str(), NULL, NULL, "main", "ps_5_0", (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG), 0, NULL,
        &pixelShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "SkydomeShaderPS Compilation Failed.");

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateVertexShader failed");

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateVertexShader failed");

    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_Layout);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateInputLayout failed");

    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateBuffer failed");

    D3D11_BUFFER_DESC colorBufferDesc;
    colorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    colorBufferDesc.ByteWidth = sizeof(ColorBufferType);
    colorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    colorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    colorBufferDesc.MiscFlags = 0;
    colorBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&colorBufferDesc, NULL, &m_ColorBuffer);
    popAssert(!FAILED(result), "SkydomeShader::InitializeShader::CreateBuffer failed");

    return true;
}

void SkydomeShader::ShutdownShader()
{
    if (m_MatrixBuffer)
    {
        m_MatrixBuffer->Release();
        m_MatrixBuffer = nullptr;
    }

    if (m_ColorBuffer)
    {
        m_ColorBuffer->Release();
        m_ColorBuffer = nullptr;
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

bool SkydomeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX& worldMatrix,
    D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix, DirectX::XMFLOAT4 apexColor, DirectX::XMFLOAT4 centerColor)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    // Transpose the matrices to prepare them for the shader.
    D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
    D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
    D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

    result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    dataPtr = (MatrixBufferType*)mappedResource.pData;

    // istoilov: nasty hack until I clean up the deprecated D3DXMATRIX 
    dataPtr->world = *reinterpret_cast<DirectX::XMMATRIX*>(&worldMatrix);
    dataPtr->view = *reinterpret_cast<DirectX::XMMATRIX*>(&viewMatrix);
    dataPtr->projection = *reinterpret_cast<DirectX::XMMATRIX*>(&projectionMatrix);

    deviceContext->Unmap(m_MatrixBuffer, 0);

    bufferNumber = 0;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

//=========================================================================================

    result = deviceContext->Map(m_ColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    ColorBufferType* colorDataPtr = (ColorBufferType*)mappedResource.pData;
    colorDataPtr->apexColor = apexColor;
    colorDataPtr->centerColor = centerColor;
    deviceContext->Unmap(m_ColorBuffer, 0);
    
    bufferNumber = 0;
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_ColorBuffer);

    return true;
}

bool SkydomeShader::Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount,
                           D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
                           DirectX::XMFLOAT4 apexColor, DirectX::XMFLOAT4 centerColor)
{
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor))
        return false;

    RenderShader(deviceContext, indexCount);

    return true;
}

void SkydomeShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_Layout);

    deviceContext->VSSetShader(m_VertexShader, NULL, 0);
    deviceContext->PSSetShader(m_PixelShader, NULL, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}