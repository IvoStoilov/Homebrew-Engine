#pragma once
#include "renderer/common/textureshader.h"

struct ReflectionBufferType
{
    DirectX::XMMATRIX m_ReflectionMatrix;
    DirectX::XMMATRIX m_ProjectionMatrix;
};

struct ReflectionShaderParams : public TextureShaderParams
{
    DirectX::XMMATRIX m_ReflectionMatrix;
};

class ReflectionShader : public TextureShader
{
    using super = TextureShader;
protected:
    virtual const String GetVSPath() const override { return "../../source/renderer/shader/reflectionVS.hlsl"; }
    virtual const String GetPSPath() const override { return "../../source/renderer/shader/reflectionPS.hlsl"; }

    virtual bool InitializeInternal(ID3D11Device* device) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;

    virtual void ShutdownInternal() override;
private:
    ID3D11Buffer* m_ReflectionMatrixBuffer = nullptr;
};