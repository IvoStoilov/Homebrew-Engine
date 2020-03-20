#pragma once
#include <system/singleton/singleton.h>

#define g_CommandLineOptions CommandLineOptions::GetInstance()

struct CommandLineArg;
class CommandLineOptions
{
    POP_DECLARE_SINGLETON(CommandLineOptions);
public:
    static void Binarize_Callback      (const CommandLineArg& arg);
    static void Fullscreen_Callback    (const CommandLineArg& arg);
    static void ShowCursor_Callback    (const CommandLineArg& arg);
    static void QuitAfterInit_Callback (const CommandLineArg& arg);
    static void DrawWireframe_Callback (const CommandLineArg& arg);

private:
    CommandLineOptions() = default;
public:
    bool m_Binarize = false;
    bool m_Fullscreen = false;
    bool m_ShowCursor = true;
    bool m_QuitAfterInit = false;
    bool m_DrawWireframe = false;

    String m_BinarizeOutputFile;
    Array<String> m_BinarizeInputFiles;

    inline String GetBinarizeInputFile(u32 idx) { return (idx < m_BinarizeInputFiles.size()) ? m_BinarizeInputFiles[idx] : ""; }
};