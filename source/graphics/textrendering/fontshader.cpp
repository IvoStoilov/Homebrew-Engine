#include <graphics/precompile.h>
#include <graphics/textrendering/fontshader.h>

#include <system/error.h>
#include <d3dcompiler.h>


using namespace std;

const string VS_SHADER_PATH = "../../source/graphics/shader/fontVS.hlsl";
const string PS_SHADER_PATH = "../../source/graphics/shader/fontPS.hlsl";

FontShader::FontShader() :
    m_VertexShader(nullptr),
    m_PixelShader(nullptr),
    m_Layout(nullptr),
    m_MatrixBuffer(nullptr),
    m_FontColorBuffer(nullptr),
    m_SampleState(nullptr)
{
}

FontShader::~FontShader()
{}

bool FontShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device, VS_SHADER_PATH, PS_SHADER_PATH);
}

void FontShader::Shutdown()
{
    ShutdownShader();
}

bool FontShader::Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, mat4x4& worldMatrix, mat4x4& viewMatrix, mat4x4& projectionMatrix, ID3D11ShaderResourceView* texture, vec4& pixelColor)
{
    // Set the shader parameters that it will use for rendering.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor))
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

bool FontShader::InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath)
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

#ifdef POP_DEBUG
    u32 shaderCompileFlags = (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS);
#elif POP_RELEASE
    u32 shaderCompileFlags = (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3);
#endif
    std::wstring unicodeVSPath(vsPath.begin(), vsPath.end());
    static constexpr D3D_SHADER_MACRO* P_DEFINES_NONE = nullptr;
    static constexpr ID3DInclude* P_INCLUDE_NONE = nullptr;
    static constexpr LPCSTR P_ENTRY_POINT_MAIN = "main";
    static constexpr LPCSTR P_TARGET_VS_5_0 = "vs_5_0";
    static constexpr u32 FLAGS_2_NONE = 0u;
    result = D3DCompileFromFile(unicodeVSPath.c_str(),
        P_DEFINES_NONE,
        P_INCLUDE_NONE,
        P_ENTRY_POINT_MAIN,
        P_TARGET_VS_5_0,
        shaderCompileFlags,
        FLAGS_2_NONE,
        &vertexShaderBuffer,
        &errorMessage);
    popAssert(!FAILED(result), "Vertex Compilation Failed.");

    std::wstring unicodePSPath(psPath.begin(), psPath.end());
    static constexpr LPCSTR P_TARGET_PS_5_0 = "ps_5_0";
    result = D3DCompileFromFile(unicodePSPath.c_str(),
        P_DEFINES_NONE,
        P_INCLUDE_NONE,
        P_ENTRY_POINT_MAIN,
        P_TARGET_PS_5_0,
        shaderCompileFlags,
        FLAGS_2_NONE,
        &pixelShaderBuffer,
        &errorMessage);

    popAssert(!FAILED(result), "Pixel Shader Compilation Failed.");

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

    // Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
    D3D11_BUFFER_DESC pixelBufferDesc;
    pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
    pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pixelBufferDesc.MiscFlags = 0;
    pixelBufferDesc.StructureByteStride = 0;

    // Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_FontColorBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void FontShader::ShutdownShader()
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

    if (m_FontColorBuffer)
    {
        m_FontColorBuffer->Release();
        m_FontColorBuffer = nullptr;
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

bool FontShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, mat4x4 worldMatrix,
    mat4x4 viewMatrix, mat4x4 projectionMatrix, ID3D11ShaderResourceView* texture, vec4& pixelColor)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    // Transpose the matrices to prepare them for the shader.
    worldMatrix = worldMatrix.Transpose();
    viewMatrix = viewMatrix.Transpose();
    projectionMatrix = projectionMatrix.Transpose();

    // Lock the constant buffer so it can be written to.
    result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    //mat4x4Identity(&projectionMatrix);
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

    // Lock the pixel constant buffer so it can be written to.
    result = deviceContext->Map(m_FontColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }
    
    // Get a pointer to the data in the pixel constant buffer.
    PixelBufferType* dataPtr2 = (PixelBufferType*)mappedResource.pData;

    // Copy the pixel color into the pixel constant buffer.
    dataPtr2->fontColor = pixelColor;

    // Unlock the pixel constant buffer.
    deviceContext->Unmap(m_FontColorBuffer, 0);

    // Set the position of the pixel constant buffer in the pixel shader.
    bufferNumber = 0;

    // Now set the pixel constant buffer in the pixel shader with the updated value.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_FontColorBuffer);

    return true;
}

void FontShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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