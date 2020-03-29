#pragma once
#include <graphics/common/gfxwindow/gfxwindowdata.h>
#include <system/viewprovider/windowcookie.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct IDXGISwapChain;
class GfxWindow
{
public:
    void Initialize(ID3D11Device* device, const GfxWindowData& windowData);
    void Shutdown();

    void SetRenderTargetView(ID3D11DeviceContext* context);
    void UnsetRenderTargetView(ID3D11DeviceContext* context);

    void PresentSwapChain();
    inline const WindowCookie& GetWindowCookie() const { return m_WindowCookie; }

private:
    void InitWindow(const GfxWindowData& windowData);
    void InitSwapChain(ID3D11Device* device, const GfxWindowData& windowData);
    void InitRenderTargetView(ID3D11Device* device);
    void InitDepthStencilView(ID3D11Device* device, const GfxWindowData& windowData);

    //Maybe move to a helper or a wrapper of the device
    IDXGIFactory2* GetFactoryFromDevice(ID3D11Device* device);
private:
    GfxWindowData m_Data;
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;
    ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
    IDXGISwapChain1* m_SwapChain = nullptr;
    WindowCookie m_WindowCookie;
};