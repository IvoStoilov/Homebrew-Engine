#pragma once
#include "renderer/common/textureshader.h"

struct ReflectionBufferType
{
    DirectX::XMMATRIX m_ReflectionMatrix;
    DirectX::XMMATRIX m_ProjectionMatrix;
};

struct DeltaTimeBufferType
{
    f32 m_MoveFactor;
    f32 padding[3];
};

struct ReflectionShaderParams : public TextureShaderParams
{
    DirectX::XMMATRIX m_ReflectionMatrix;
    f32 m_MoveFactor;
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
    void UpdateMoveFactor(f32 dt);

    f32 m_MoveFactor = 0.f;
    ID3D11Buffer* m_ReflectionMatrixBuffer = nullptr;
    ID3D11Buffer* m_DeltaTImeBuffer = nullptr;
};