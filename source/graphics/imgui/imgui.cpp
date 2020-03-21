#include <graphics/precompile.h>
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/base/externimgui.h>

#include <system/viewprovider/viewprovider.h>

#ifdef POP_IMGUI_ENABLED
bool ImGuiRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_ViewProvider.GetWindowHandle());
    ImGui_ImplDX11_Init(device, deviceContext);

    return true;
}

bool ImGuiRenderer::Render(D3D11* d3d)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return true;
}

void ImGuiRenderer::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#endif //POP_IMGUI_ENABLED