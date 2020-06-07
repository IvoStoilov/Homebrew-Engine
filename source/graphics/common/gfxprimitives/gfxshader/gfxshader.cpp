#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxshader/gfxshader.h>

#include <d3dcompiler.h>

void GfxShader::LoadBytecodeFromFile(const String& bytecodePath)
{
    std::wstring unicodePath(bytecodePath.begin(), bytecodePath.end());
    popGfxVerify(D3DReadFileToBlob(unicodePath.c_str(), &m_ShaderBytecode));
}

void GfxShader::CompileBytecode(const String& shaderSourcePath, Target target)
{
#ifdef POP_DEBUG
    constexpr u32 SHADER_COMPILE_FLAGS = (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS);
#elif POP_RELEASE
    constexpr u32 SHADER_COMPILE_FLAGS = (D3D10_SHADER_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3);
#endif
    ComPtr<ID3D10Blob> errorBlob;

    std::wstring unicodePath(shaderSourcePath.begin(), shaderSourcePath.end());
    static constexpr D3D_SHADER_MACRO* P_DEFINES_NONE = nullptr;
    static constexpr ID3DInclude* P_INCLUDE_NONE = nullptr;
    static constexpr LPCSTR P_ENTRY_POINT_MAIN = "main";
    static const LPCSTR P_TARGET = TargetToString(target);
    static constexpr u32 FLAGS_2_NONE = 0u;
    HRESULT result = D3DCompileFromFile(unicodePath.c_str(),
        P_DEFINES_NONE,
        P_INCLUDE_NONE,
        P_ENTRY_POINT_MAIN,
        P_TARGET,
        SHADER_COMPILE_FLAGS,
        FLAGS_2_NONE,
        &m_ShaderBytecode,
        &errorBlob);

#ifdef POP_ASSERT_ENABLED
    if (FAILED(result))
    {
        const u32 size = u32(errorBlob->GetBufferSize());
        UniquePtr<char> errorMsgBuffer(new char[size]);
        memcpy(errorMsgBuffer.get(), errorBlob->GetBufferPointer(), size);
        popAssert(false, "VS {} failed compilation. Reason : {}", shaderSourcePath.c_str(), errorMsgBuffer.get());
    }
#endif //POP_ASSERT_ENABLED
}

constexpr char* GfxShader::TargetToString(Target target)
{
    switch (target)
    {
    case Target::VS_5_0: return "vs_5_0";
    case Target::PS_5_0: return "ps_5_0";
    default:
        popAssert(false, "Target not supported");
    }
    return "";
}
//===================================================================================
GfxVertexShader::GfxVertexShader(ComPtr<ID3D11Device>& device, const String& path, GfxShader::PathType pathType)
{
    switch (pathType)
    {
        case GfxShader::PathType::BytecodePath : LoadBytecodeFromFile(path); break;
        case GfxShader::PathType::SourcecodePath: CompileBytecode(path, GfxShader::Target::VS_5_0); break;
        default:
            popAssert(false, "Shader Path Type not supported");
    }

    popGfxVerify(device->CreateVertexShader(m_ShaderBytecode->GetBufferPointer(), m_ShaderBytecode->GetBufferSize(), nullptr, &m_VertexShader));
}

void GfxVertexShader::Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const
{
    deviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
}
//===================================================================================
GfxPixelShader::GfxPixelShader(ComPtr<ID3D11Device>& device, const String& path, GfxShader::PathType pathType)
{
    switch (pathType)
    {
    case GfxShader::PathType::BytecodePath: LoadBytecodeFromFile(path); break;
    case GfxShader::PathType::SourcecodePath: CompileBytecode(path, GfxShader::Target::PS_5_0); break;
    default:
        popAssert(false, "Shader Path Type not supported");
    }

    popGfxVerify(device->CreatePixelShader(m_ShaderBytecode->GetBufferPointer(), m_ShaderBytecode->GetBufferSize(), nullptr, &m_PixelShader));

}

void GfxPixelShader::Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const
{
    deviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}
