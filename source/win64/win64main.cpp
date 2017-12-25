#include "win64/win64viewprovider.h"
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    Win64_ViewProvider* viewProvider;
    bool result;


    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    // Create the system object.
    viewProvider = new Win64_ViewProvider();
    if (!viewProvider)
    {
        return 0;
    }

    // Initialize and run the system object.
    result = viewProvider->Initialize();
    if (result)
    {
        viewProvider->Run();
    }

    // Shutdown and release the system object.
    viewProvider->Shutdown();
    delete viewProvider;
    viewProvider = nullptr;

    return 0;
}
