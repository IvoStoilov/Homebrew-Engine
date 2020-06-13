#include <graphics/precompile.h>
#include <graphics/common/legacyshaders/colorshader.h>
#include <system/error.h>

#include <d3dcompiler.h>

/*
Shader Paths :
source/renderer/shader/simpleVS.hlsl
source/renderer/shader/simplePS.hlsl

source/renderer/shader/textureVS.hlsl
source/renderer/shader/texturePS.hlsl
*/
using namespace std;

const string VS_SHADER_PATH = "../../source/graphics/shader/simpleVS.hlsl";
const string PS_SHADER_PATH = "../../source/graphics/shader/simplePS.hlsl";

ColorShader::ColorShader() :
    m_VertexShader(nullptr),
    m_PixelShader(nullptr),
    m_Layout(nullptr),
    m_MatrixBuffer(nullptr)
{
}

ColorShader::~ColorShader()
{}

bool ColorShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device, VS_SHADER_PATH, PS_SHADER_PATH);
}

void ColorShader::Shutdown()
{
    ShutdownShader();
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, mat4x4 worldMatrix, mat4x4 viewMatrix, mat4x4 projectionMatrix)
{
    // Set the shader parameters that it will use for rendering.
    if(!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
        return false;

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}

bool ColorShader::InitializeShader(ID3D11Device* device, const std::string& vsPath, const std::string& psPath)
{
    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
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
    popAssert(!FAILED(result), "ColorShaderVS Compilation Failed.");

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
    popAssert(!FAILED(result), "ColorShaderPS Compilation Failed.");

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
    popAssert(!FAILED(result), "ColorShader::InitializeShader::CreateVertexShader failed");

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
    popAssert(!FAILED(result), "ColorShader::InitializeShader::CreateVertexShader failed");


    // Now setup the layout of the data that goes into the shader.
    // This setup needs to match the VertexType stucture in the ModelClass and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;
    
    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_Layout);
    popAssert(!FAILED(result), "ColorShader::InitializeShader::CreateInputLayout failed");

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
    popAssert(!FAILED(result), "ColorShader::InitializeShader::CreateBuffer failed");

    return true;
}

void ColorShader::ShutdownShader()
{
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

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, mat4x4& worldMatrix,
    mat4x4& viewMatrix, mat4x4& projectionMatrix)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;

    mat4x4 worldMatrixTransposed = worldMatrix.GetTransposed();
    mat4x4 viewMatrixTransposed = viewMatrix.GetTransposed();
    mat4x4 projectionMatrixTransposed = projectionMatrix.GetTransposed();

    result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    //mat4x4Identity(&projectionMatrix);
    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrixTransposed;
    dataPtr->view = viewMatrixTransposed;
    dataPtr->projection = projectionMatrixTransposed;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_MatrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 0;

    // Finanly set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

    return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_Layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_VertexShader, NULL, 0);
    deviceContext->PSSetShader(m_PixelShader, NULL, 0);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}