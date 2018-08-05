#pragma once
#include <string>
#include <vector>

#define g_CommandLineOptions CommandLineOptions::GetInstance()

struct CommandLineArg;
class CommandLineOptions
{
public:
    bool m_Binarize;
    bool m_Fullscreen;
    bool m_ShowCursor;
    bool m_QuitAfterInit;
    bool m_DrawWireframe;

    std::string m_BinarizeOutputFile;
    std::vector<std::string> m_BinarizeInputFiles;

public:
    static void Binarize_Callback      (const CommandLineArg& arg);
    static void Fullscreen_Callback    (const CommandLineArg& arg);
    static void ShowCursor_Callback    (const CommandLineArg& arg);
    static void QuitAfterInit_Callback (const CommandLineArg& arg);
    static void DrawWireframe_Callback (const CommandLineArg& arg);

public:
    static CommandLineOptions* GetInstance();
    static void CreateInstance();
    static void CleanInstance();
private:
    static CommandLineOptions* s_Instance;
    CommandLineOptions();
};