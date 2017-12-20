#include "renderer/lightshader.h"
#include "system/error.h"

#include <d3d11.h>
#include <D3DCompiler.h>
#include <d3dx11async.h>

using namespace std;

const string VS_SHADER_PATH = "source/renderer/shader/diffuselightingVS.hlsl";
const string PS_SHADER_PATH = "source/renderer/shader/diffuselightingPS.hlsl";

LightShader::LightShader() :
    m_VertexShader(nullptr),
    m_PixelShader(nullptr),
    m_Layout(nullptr),
    m_MatrixBuffer(nullptr),
    m_SampleState(nullptr),
    m_LightBuffer(nullptr)
{}

LightShader::~LightShader()
{}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
    return InitializeShader(device, hwnd, VS_SHADER_PATH, PS_SHADER_PATH);
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, const std::string& vsPath, const std::string& psPath)
{
    HRESULT result;
    
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    D3D11_SAMPLER_DESC samplerDesc;

    unsigned int numElements;

    // Initialize the pointers this function will use to null.
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    result = D3DX11CompileFromFile(vsPath.c_str(), NULL, NULL, "main", "vs_5_0", (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG), 0, NULL,
        &vertexShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "LightShaderVS Compilation Failed.");
    
    result = D3DX11CompileFromFile(psPath.c_str(), NULL, NULL, "main", "ps_5_0", (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG), 0, NULL,
        &pixelShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "LightShaderPS Compilation Failed.");

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    popAssert(!FAILED(result), "LightShader::InitializeShader::CreateVertexShader failed");

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    popAssert(!FAILED(result), "LightShader::InitializeShader::CreatePixelShader failed");

    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_Layout);

    popAssert(!FAILED(result), "LightShader::InitializeShader::CreateInputLayout failed");

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    D3D11_BUFFER_DESC matrixBufferDesc;
    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
    popAssert(!FAILED(result), "LightShader::InitializeShader::CreateBuffer failed");

    D3D11_BUFFER_DESC lightBufferDesc;
    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&lightBufferDesc, NULL, &m_LightBuffer);
    popAssert(!FAILED(result), "LightShader::InitializeShader::CreateBuffer failed");

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

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_SampleState);
    popAssert(!FAILED(result), "LightShader::InitializeShader::CreateSamplerState failed");

    return true;
}

void LightShader::Shutdown()
{
    ShutdownShader();
}

void LightShader::ShutdownShader()
{
    // Release the sampler state.
    if (m_SampleState)
    {
        m_SampleState->Release();
        m_SampleState = 0;
    }

    // Release the matrix constant buffer.
    if (m_MatrixBuffer)
    {
        m_MatrixBuffer->Release();
        m_MatrixBuffer = 0;
    }

    if (m_LightBuffer)
    {
        m_LightBuffer->Release();
        m_LightBuffer = 0;
    }
    // Release the layout.
    if (m_Layout)
    {
        m_Layout->Release();
        m_Layout = 0;
    }

    // Release the pixel shader.
    if (m_PixelShader)
    {
        m_PixelShader->Release();
        m_PixelShader = 0;
    }

    // Release the vertex shader.
    if (m_VertexShader)
    {
        m_VertexShader->Release();
        m_VertexShader = 0;
    }
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
    D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix,
    ID3D11ShaderResourceView* texture,
    D3DXVECTOR4& diffuseColor,
    D3DXVECTOR4& lightDirection)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    unsigned int bufferNumber = 0;
    MatrixBufferType* matrixData = nullptr;
    LightBufferType* lightData = nullptr;

    // Transpose the matrices to prepare them for the shader.
    D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
    D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
    D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

    // Lock the constant buffer so it can be written to.
    result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    popAssert(!FAILED(result), "LightShader::SetShaderParameters::Matrix Mapping failed.");
    
    matrixData = (MatrixBufferType*)mappedResource.pData;
    matrixData->world = worldMatrix;
    matrixData->view = viewMatrix;
    matrixData->projection = projectionMatrix;

    deviceContext->Unmap(m_MatrixBuffer, 0);

    result = deviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    popAssert(!FAILED(result), "LightShader::SetShaderParameters::Light Mapping failed.");

    lightData = (LightBufferType*)mappedResource.pData;
    lightData->diffuseColor = diffuseColor;
    lightData->lightDirection = lightDirection;

    deviceContext->Unmap(m_LightBuffer, 0);

    matrixData = nullptr;
    lightData = nullptr;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_LightBuffer);
    deviceContext->PSSetShaderResources(0, 1, &texture);

    return true;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount,
    D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix,
    ID3D11ShaderResourceView* texture,
    D3DXVECTOR4& diffuseColor,
    D3DXVECTOR4& lightDirection)
{
    // Set the shader parameters that it will use for rendering.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, diffuseColor, lightDirection))
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, uint32_t indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_Layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_VertexShader, NULL, 0);
    deviceContext->PSSetShader(m_PixelShader, NULL, 0);

    // Set the sampler state in the pixel shader.
    deviceContext->PSSetSamplers(0, 1, &m_SampleState);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}