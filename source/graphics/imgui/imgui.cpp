#include <graphics/precompile.h>
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/base/externimgui.h>
#include <graphics/d3d11.h>

#include <system/viewprovider/viewprovider.h>

#ifdef POP_IMGUI_ENABLED
bool ImGuiRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    GfxWindowData gfxWindowData;
    gfxWindowData.m_WindowData.m_WindowName = "ImGui Window";
    gfxWindowData.m_WindowData.m_WindowResolution.w = 680u;
    gfxWindowData.m_WindowData.m_WindowResolution.h = 550u;
    m_Window.Initialize(device, gfxWindowData);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui::SetNextWindowPos(ImVec2::ImVec2(0, 0), ImGuiCond_Always);
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_ViewProvider.GetWindow(m_Window.GetWindowCookie()).m_WindowHandle);
    ImGui_ImplDX11_Init(device, deviceContext);

    return true;
}

bool ImGuiRenderer::Render(D3D11* d3d)
{
    m_Window.SetRenderTargetView(d3d->GetDeviceContext_DEPRECATED());
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    m_Window.UnsetRenderTargetView(d3d->GetDeviceContext_DEPRECATED());

    return true;
}

void ImGuiRenderer::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    m_Window.Shutdown();
}
#endif //POP_IMGUI_ENABLED