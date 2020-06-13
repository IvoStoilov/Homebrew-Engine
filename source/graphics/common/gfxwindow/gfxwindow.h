#pragma once
#include <graphics/common/gfxwindow/gfxwindowdata.h>
#include <system/viewprovider/windowcookie.h>

class Color;
class GfxWindow
{
public:
    void Initialize(ComPtr<ID3D11Device>& device, const GfxWindowData& windowData);
    void Shutdown();

    void BeginFrame(ComPtr<ID3D11DeviceContext>& context, const Color& clearColor);
    void EndFrame();

    void SetRenderTargetView(ComPtr<ID3D11DeviceContext>& context);
    void UnsetRenderTargetView(ComPtr<ID3D11DeviceContext>& context);

    inline const WindowCookie& GetWindowCookie() const { return m_WindowCookie; }

private:
    void InitWindow(const GfxWindowData& gfxWindowData);
    void InitSwapChain(ComPtr<ID3D11Device>& device, const GfxWindowData& windowData, const Resolution& renderingResolution);
    void InitRenderTargetView(ComPtr<ID3D11Device>& device);
    void InitDepthStencilView(ComPtr<ID3D11Device>& device, const GfxWindowData& windowData, const Resolution& renderingResolution);

    //Maybe move to a helper or a wrapper of the device
    IDXGIFactory2* GetFactoryFromDevice(ID3D11Device* device);
private:
    GfxWindowData m_Data;
    ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
    ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
    ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
    ComPtr<IDXGISwapChain1> m_SwapChain;
    WindowCookie m_WindowCookie;
};