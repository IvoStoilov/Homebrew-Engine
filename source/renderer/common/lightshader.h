#pragma once
#include "renderer/common/textureshader.h"

struct LightShaderParams : public TextureShaderParams
{
    DirectX::XMFLOAT4 m_DiffuseColor;
    DirectX::XMFLOAT4 m_LightDirection;
    DirectX::XMFLOAT4 m_AmbientLight;
};

class LightShader : public TextureShader
{
    using super = TextureShader;
public:
    LightShader();
    LightShader(const String& vsPath, const String& psPath);

protected:
    struct LightBufferType
    {
        DirectX::XMFLOAT4 m_DiffuseColor;
        DirectX::XMFLOAT4 m_LightDirection;
        DirectX::XMFLOAT4 m_AmbientLight;
    };

protected:
    virtual const String GetVSPath() const override { return m_VSPath; }
    virtual const String GetPSPath() const override { return m_PSPath; }

    virtual bool InitializeInternal(ID3D11Device* device) override;
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) override;

    virtual void AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd) override;
    virtual void ShutdownInternal() override;
    
protected:
    ID3D11Buffer* m_LightBuffer = nullptr;
    String m_VSPath;
    String m_PSPath;
};
