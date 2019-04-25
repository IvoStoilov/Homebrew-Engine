#pragma once
#include "renderer/common/baseshader.h"
#include "renderer/common/texture.h"

struct TextureShaderParams : public ShaderParamsBase
{
    Array<SharedPtr<Texture>> m_Textures;

    Array<ID3D11ShaderResourceView*> GetShaderResourceViewArray() const
    {
        Array<ID3D11ShaderResourceView*> result;
        for (const SharedPtr<Texture>& texture : m_Textures)
        {
            result.push_back(texture.get()->GetTexture());
        }
        return result;
    }
};

class TextureShader : public BaseShader
{
protected:
    virtual const String GetVSPath() const override { return "../../source/renderer/shader/textureVS.hlsl"; }
    virtual const String GetPSPath() const override { return "../../source/renderer/shader/texturePS.hlsl"; }

    virtual bool InitializeInternal(ID3D11Device* device) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;

    virtual void AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd) override;
    virtual void ShutdownInternal() override;

protected:
    ID3D11SamplerState* m_SampleState;
};