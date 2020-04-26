#pragma once
struct ID3D11DeviceContext;
class GfxObject
{
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const = 0;
};