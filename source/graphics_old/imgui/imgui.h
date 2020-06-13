#pragma once
#ifdef POP_IMGUI_ENABLED
#include <graphics/isubrenderer.h>
#include <graphics/common/gfxwindow/gfxwindow.h>

#include <system/viewprovider/windowcookie.h>

struct ID3D11DeviceContext;
class ImGuiRenderer : public ISubRenderer
{
    using RenderCallback = std::function<void()>;
public:
    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
    virtual bool Render(D3D11* d3d) override;
    virtual void Shutdown() override;

    u32 RegisterRenderCallbackAndReturnIndex(RenderCallback&& callback);
    void RemoveRenderCallback(u32 index);
private:
    void RenderGameWindowOverlays(ID3D11DeviceContext* deviceContext);
    void RenderSeparateWindow(ID3D11DeviceContext* deviceContext);

private:
    Array<RenderCallback> m_RenderCallbacks;
    GfxWindow m_Window;
};
#endif //POP_IMGUI_ENABLED