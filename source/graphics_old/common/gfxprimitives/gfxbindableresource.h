#pragma once
struct ID3D11DeviceContext;
class GfxBindableResource
{
public:
    virtual ~GfxBindableResource() = default;
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const = 0;
};