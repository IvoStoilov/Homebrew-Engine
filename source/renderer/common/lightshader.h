#pragma once
#include "renderer/common/textureshader.h"

struct LightShaderParams : public TextureShaderParams
{
    DirectX::XMFLOAT4 m_DiffuseColor;
    DirectX::XMFLOAT4 m_LightDirection;
};

class LightShader : public TextureShader
{
    using super = TextureShader;
protected:
    struct LightBufferType
    {
        DirectX::XMFLOAT4 m_DiffuseColor;
        DirectX::XMFLOAT4 m_LightDirection;
    };

protected:
    virtual const String GetVSPath() const override { return "../../source/renderer/shader/diffuselightingVS.hlsl"; }
    virtual const String GetPSPath() const override { return "../../source/renderer/shader/diffuselightingPS.hlsl"; }

    virtual bool InitializeInternal(ID3D11Device* device) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;

    virtual void AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd) override;
    virtual void ShutdownInternal() override;
    
protected:
    ID3D11Buffer* m_LightBuffer;
};
