#pragma once
#include <d3d11.h>
#include <directxmath.h>

struct MatrixBufferType
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

struct ShaderParamsBase
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;

    
    template<class T>
    const T* GetAs() const { return popObjectDynamicCast(const T, this); }

private:
    //istoilov : C++ needs at least one virtual function to generate RTTI and enable downcasting :/
    virtual void Dummy() {};
};

class BaseShader
{
public:
    bool Initialize(ID3D11Device* device);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext, uint32_t indexCount, const ShaderParamsBase& shaderParams);

protected:
    virtual const String GetVSPath() const = 0;
    virtual const String GetPSPath() const = 0;

    virtual bool InitializeInternal(ID3D11Device* device, const UniquePtr<ID3D10Blob>& vsBlob, const UniquePtr<ID3D10Blob>& psBlob) { return true; }
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) { return true; }
    virtual void ShutdownInternal() {}
    
private:
    using VS_PS_Blobs = std::pair< UniquePtr<ID3D10Blob>, UniquePtr<ID3D10Blob> >;
    VS_PS_Blobs CompileShaders(ID3D11Device* device, const String&& vsPath, const String&& psPath);
    void InitializeMatrixBuffer(ID3D11Device* device);

    void SetShaderParameters(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams);
    void SetMatrixBuffer(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams);
    void RenderShader(ID3D11DeviceContext* deviceContext, uint32_t indexCount);

protected:
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_Layout = nullptr;

    ID3D11Buffer* m_MatrixBuffer = nullptr;
};