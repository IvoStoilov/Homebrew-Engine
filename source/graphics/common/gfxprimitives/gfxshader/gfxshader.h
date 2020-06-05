#include <graphics/common/gfxprimitives/gfxbindableresource.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexlayout.h>

class GfxPixelShader : public GfxBindableResource
{
    void Initialize(const String& path, const GfxVertexLayout& vertexLayout);
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
private:
    String m_Path;
    Array<SharedPtr<GfxBindableResource>> m_ConstBuffers;
    ComPtr<ID3D11PixelShader> m_PixelShader;
};