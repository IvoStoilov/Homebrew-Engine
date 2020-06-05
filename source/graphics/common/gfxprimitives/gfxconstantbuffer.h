#pragma once
#include <graphics/common/gfxprimitives/gfxbindableresource.h>

template <class T>
class GfxConstantBuffer : public GfxBindableResource
{
public:
    inline GfxConstantBuffer(ComPtr<ID3D11Device>& device, const T& data, u32 slot = 0u);
    inline GfxConstantBuffer(ComPtr<ID3D11Device>& device, u32 slot = 0);
    inline void Update(ComPtr<ID3D11DeviceContext>& deviceContext, const T& data);

private:
    ComPtr<ID3D11Buffer> m_ConstantBuffer;
    u32 m_Slot = 0u;
};

//======================================================================================================
template <class T>
class GfxVertexConstantBuffer : public GfxConstantBuffer<T>
{
public:
    inline virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) override;
};
//======================================================================================================
template <class T>
class GfxPixelConstantBuffer : public GfxConstantBuffer<T>
{
public:
    inline virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) override;
};

#include <graphics/common/gfxprimitives/gfxconstantbuffer.hpp>