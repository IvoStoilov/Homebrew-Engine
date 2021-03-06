#include <gameplay/precompile.h>

#include <engine/engine.h>

#include <system/commandline/commandlineparser.h>
#include <system/commandline/commandlineoptions.h>

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

    Engine::CreateInstance();
    Engine::GetInstance().Update();
    Engine::CleanInstance();

    return 0;
}