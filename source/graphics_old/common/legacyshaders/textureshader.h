#pragma once
#include <graphics/common/legacyshaders/baseshader.h>
#include <graphics/common/gfxprimitives/gfxtexture.h>

struct TextureShaderParams : public ShaderParamsBase
{
    using TextureIndexPair = std::pair<SharedPtr<GfxTexture>, u32>;

    Array<TextureIndexPair> m_PSTextures;
    Array<TextureIndexPair> m_VSTextures;

    static Array<ID3D11ShaderResourceView*> GetShaderResourceViewArray(const Array<SharedPtr<GfxTexture>>& textures)
    {
        Array<ID3D11ShaderResourceView*> result;
        for (const SharedPtr<GfxTexture>& texture : textures)
        {
            //result.push_back(texture.get()->GetTexture());
        }
        return result;
    }
};

class TextureShader : public BaseShader
{
protected:
    virtual const String GetVSPath() const override { return "../../source/graphics/shader/textureVS.hlsl"; }
    virtual const String GetPSPath() const override { return "../../source/graphics/shader/texturePS.hlsl"; }

    virtual bool InitializeInternal(ID3D11Device* device) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;
    virtual void UnSetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;

    virtual void AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd) override;
    virtual void ShutdownInternal() override;

protected:
    ID3D11SamplerState* m_VSSampleState;
    ID3D11SamplerState* m_PSSampleState;
};