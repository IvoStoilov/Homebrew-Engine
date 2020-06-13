#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxobject.h>
#include <graphics/common/gfxprimitives/gfxbindableresource.h>
#include <graphics/common/gfxprimitives/gfxindexbuffer.h>

void GfxObject::AddBindableResource(const SharedPtr<GfxBindableResource>& resource)
{
    m_BindableResources.push_back(resource);
    if (const GfxIndexBuffer* indexBuffer = popObjectDynamicCast(GfxIndexBuffer, resource.get()))
    {
        m_IndexCount = indexBuffer->GetSize();
    }
}

void GfxObject::BindResourcesAndDraw(ComPtr<ID3D11DeviceContext>& deviceContext)
{
    for (const SharedPtr<GfxBindableResource>& resource : m_BindableResources)
    {
        resource->Bind(deviceContext);
    }
    deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}
