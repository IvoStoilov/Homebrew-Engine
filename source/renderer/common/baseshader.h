#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "system/math/vec4.h"

struct MatrixBufferType
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

struct ClipPlaneBufferType
{
    DirectX::XMVECTOR clipPlane;
};

struct ShaderParamsBase
{
    DirectX::XMMATRIX m_World;
    DirectX::XMMATRIX m_View;
    DirectX::XMMATRIX m_Projection;

    bool m_IsClipPlaneEnable = false;
    DirectX::XMVECTOR m_ClipPlane;

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

    virtual bool InitializeInternal(ID3D11Device* device) { return true; }
    virtual bool SetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) { return true; }
    virtual void UnSetShaderParametersInternal(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams) {}
    virtual void ShutdownInternal() {}

    virtual void AddPolygonLayout(Array<D3D11_INPUT_ELEMENT_DESC>& polygonLayoutsToAdd) {}
    
private:
    using VS_PS_Blobs = std::pair< UniquePtr<ID3D10Blob>, UniquePtr<ID3D10Blob> >;
    VS_PS_Blobs CompileShaders(ID3D11Device* device, const String&& vsPath, const String&& psPath);
    void InitializeMatrixBuffer(ID3D11Device* device);
    void InitializeClipPlaneBuffer(ID3D11Device* device);
    void InitializeLayout(ID3D11Device* device, const UniquePtr<ID3D10Blob>& vsBlob, const UniquePtr<ID3D10Blob>& psBlob);

    void SetShaderParameters(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams);
    void UnSetShaderParameters(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams);
    void SetMatrixBuffer(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams);
    void SetClipPlaneBuffer(ID3D11DeviceContext* deviceContext, const ShaderParamsBase& shaderParams);

    void RenderShader(ID3D11DeviceContext* deviceContext, uint32_t indexCount);

protected:
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_Layout = nullptr;

    ID3D11Buffer* m_MatrixBuffer = nullptr;
    ID3D11Buffer* m_ClipPlaneBuffer = nullptr;
};