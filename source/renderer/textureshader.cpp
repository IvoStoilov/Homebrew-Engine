#include "renderer/textureshader.h"
#include "system/error.h"

#include <d3d11.h>
#include <D3DCompiler.h>
#include <d3dx11async.h>

using namespace std;

const string VS_SHADER_PATH = "../../source/renderer/shader/textureVS.hlsl";
const string PS_SHADER_PATH = "../../source/renderer/shader/texturePS.hlsl";

TextureShader::TextureShader() :
    m_VertexShader(nullptr),
    m_PixelShader(nullptr),
    m_Layout(nullptr),
    m_MatrixBuffer(nullptr),
    m_SampleState(nullptr)
{
}

TextureShader::~TextureShader()
{}

bool TextureShader::Initialize(ID3D11Device* device, HWND hwnd)
{
    return InitializeShader(device, hwnd, VS_SHADER_PATH, PS_SHADER_PATH);
}

void TextureShader::Shutdown()
{
    ShutdownShader();
}

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, D3DXMATRIX& worldMatrix, D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
    // Set the shader parameters that it will use for rendering.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture))
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

bool TextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, const std::string& vsPath, const std::string& psPath)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    D3D11_SAMPLER_DESC samplerDesc;

    unsigned int numElements;

    // Initialize the pointers this function will use to null.
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;


    // Compile the vertex shader code.
    result = D3DX11CompileFromFile(vsPath.c_str(), NULL, NULL, "main", "vs_5_0", (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG), 0, NULL,
        &vertexShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "Texture Shader VS compilation failed.");

    // Compile the pixel shader code.
    result = D3DX11CompileFromFile(psPath.c_str(), NULL, NULL, "main", "ps_5_0", (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG), 0, NULL,
        &pixelShaderBuffer, &errorMessage, NULL);
    popAssert(!FAILED(result), "Texture Shader PS compilation failed.")
   
    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    popAssert(!FAILED(result), "TextureShader::InitializeShader::CreateVertexShader failed");

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    popAssert(!FAILED(result), "TextureShader::InitializeShader::CreatePixelShader failed");

    // Now setup the layout of the data that goes into the shader.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_Layout);
    if (FAILED(result))
    {
        return false;
    }

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
    if (FAILED(result))
    {
        return false;
    }

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
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void TextureShader::ShutdownShader()
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

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX& worldMatrix,
    D3DXMATRIX& viewMatrix, D3DXMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    // Transpose the matrices to prepare them for the shader.
    D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
    D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
    D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

    // Lock the constant buffer so it can be written to.
    result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    //D3DXMatrixIdentity(&projectionMatrix);
    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_MatrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    
    return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_Layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_VertexShader, NULL, 0);
    deviceContext->PSSetShader(m_PixelShader, NULL, 0);

    deviceContext->PSSetSamplers(0, 1, &m_SampleState);
    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}