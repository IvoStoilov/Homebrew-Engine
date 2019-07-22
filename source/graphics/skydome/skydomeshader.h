#pragma once
#include "graphics/common/baseshader.h"
#include <directxmath.h>

struct SkydomeShaderParams : public ShaderParamsBase
{
    DirectX::XMFLOAT4 m_ApexColor;
    DirectX::XMFLOAT4 m_CenterColor;
};

class SkydomeShader : public BaseShader
{
private:
    struct ColorBufferType
    {
        DirectX::XMFLOAT4 apexColor;
        DirectX::XMFLOAT4 centerColor;
    };

protected:
    virtual const String GetVSPath() const override { return "../../source/graphics/shader/skydomeVS.hlsl"; }
    virtual const String GetPSPath() const override { return "../../source/graphics/shader/skydomePS.hlsl"; }

    virtual bool InitializeInternal(ID3D11Device* device) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;

    virtual void AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd) override;
    virtual void ShutdownInternal() override;

protected:
    ID3D11Buffer* m_ColorBuffer = nullptr;
};