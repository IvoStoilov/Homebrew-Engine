#include <graphics/common/gfxprimitives/gfxbindableresource.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>

class GfxShader : public GfxBindableResource
{
public:
    enum class PathType
    {
        BytecodePath,
        SourcecodePath
    };
    enum class Target
    {
        VS_5_0,
        PS_5_0
    };

    const ComPtr<ID3D10Blob>& GetShaderBytecode() const { return m_ShaderBytecode; }
protected:
    void LoadBytecodeFromFile(const String& bytecodePath);
    void CompileBytecode(const String& shaderSourcePath, Target target);

    static constexpr char* TargetToString(Target target);

    ComPtr<ID3D10Blob> m_ShaderBytecode;
};
//===================================================================================
class GfxVertexShader : public GfxShader
{
public:
    GfxVertexShader(ComPtr<ID3D11Device>& device, const String& path, GfxShader::PathType pathType);
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
private:
    ComPtr<ID3D11VertexShader> m_VertexShader;
};
//===================================================================================
class GfxPixelShader : public GfxShader
{
public:
    GfxPixelShader(ComPtr<ID3D11Device>& device, const String& path, GfxShader::PathType pathType);
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
private:
    ComPtr<ID3D11PixelShader> m_PixelShader;
};