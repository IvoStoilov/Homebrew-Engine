#pragma once
#ifdef POP_IMGUI_ENABLED
#include <graphics/isubrenderer.h>
#include <system/viewprovider/windowcookie.h>
class ImGuiRenderer : public ISubRenderer
{
public:
    virtual bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
    virtual bool Render(D3D11* d3d) override;
    virtual void Shutdown() override;

private:
    WindowCookie m_WindowCookie;
};
#endif //POP_IMGUI_ENABLED