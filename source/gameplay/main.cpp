#include "precompile.h"
#include <engine/win64/win64viewprovider.h>
#include <system/commandline/commandlineparser.h>
#include <system/commandline/commandlineoptions.h>
#include <system/error.h>
#include <stdio.h>
#include <windows.h>
const std::string INI_PATH = "../homebrew.ini";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    //AllocConsole();
    //freopen("CONIN$", "r", stdin);
    //freopen("CONOUT$", "w", stdout);
    //freopen("CONOUT$", "w", stderr);

    CommandLineOptions::CreateInstance();
    CommandLineParser parser;

    parser.ParseCommandLineArgsFromIni(INI_PATH);
    parser.ParseCommandLineArgs(pScmdline);
    parser.CallCallbacks();

    // Create the system object.
    Win64_ViewProvider* viewProvider = new Win64_ViewProvider();
    popAssert(viewProvider != nullptr, "Win64_ViewProvider memory alloc failed.");
    popAssert(viewProvider->Initialize(), "Win64_ViewProvider::Initialize failed.");

    viewProvider->Run();

    // Shutdown and release the system object.
    viewProvider->Shutdown();
    delete viewProvider;

    return 0;
}