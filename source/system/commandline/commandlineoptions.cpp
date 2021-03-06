#include <system/precompile.h>
#include "system/commandline/commandlineoptions.h"
#include "system/commandline/commandlineparser.h"
#include "system/error.h"

void CommandLineOptions::Binarize_Callback(const CommandLineArg& arg)
{
    g_CommandLineOptions.m_Binarize = true;
    g_CommandLineOptions.m_BinarizeOutputFile = arg.m_SubArgs[0];
    for (uint32_t i = 1; i < arg.m_SubArgs.size(); ++i)
    {
        g_CommandLineOptions.m_BinarizeInputFiles.push_back(arg.m_SubArgs[i]);
    }
}

void CommandLineOptions::Fullscreen_Callback(const CommandLineArg& arg)
{
    g_CommandLineOptions.m_Fullscreen = true;
}

void CommandLineOptions::ShowCursor_Callback(const CommandLineArg& arg)
{
    g_CommandLineOptions.m_ShowCursor = true;
}

void CommandLineOptions::QuitAfterInit_Callback(const CommandLineArg& arg)
{
    g_CommandLineOptions.m_QuitAfterInit = true;
}

void CommandLineOptions::DrawWireframe_Callback(const CommandLineArg& arg)
{
    g_CommandLineOptions.m_DrawWireframe = true;
}