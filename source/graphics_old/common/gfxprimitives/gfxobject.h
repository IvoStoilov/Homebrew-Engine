#pragma once
class GfxBindableResource;
class GfxObject
{
public:
    void AddBindableResource(const SharedPtr<GfxBindableResource>& resource);
    void BindResourcesAndDraw(ComPtr<ID3D11DeviceContext>& deviceContext);

private:
    u32 m_IndexCount = 0u;
    Array<SharedPtr<GfxBindableResource>> m_BindableResources;
};

