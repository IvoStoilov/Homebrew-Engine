#pragma once
#include "renderer/common/baseshader.h"
#include <directxmath.h>

struct SkydomeShaderParams : public ShaderParamsBase
{
    DirectX::XMFLOAT4 apexColor;
    DirectX::XMFLOAT4 centerColor;
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
    virtual const String GetVSPath() const override { return "../../source/renderer/shader/skydomeVS.hlsl"; }
    virtual const String GetPSPath() const override { return "../../source/renderer/shader/skydomePS.hlsl"; }

    virtual bool InitializeInternal(ID3D11Device* device, const UniquePtr<ID3D10Blob>& vsBlob, const UniquePtr<ID3D10Blob>& psBlob) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;
    virtual void ShutdownInternal() override;

private:
    ID3D11Buffer* m_ColorBuffer = nullptr;
};