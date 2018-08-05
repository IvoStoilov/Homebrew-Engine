#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include "system/commandline/commandlineoptions.h"

struct CommandLineArg;
class CommandLineParser
{
public:
    CommandLineParser();
    void RegisterCommandLineArgs();

    void ParseCommandLineArgs(char* args);
    void ParseCommandLineArgsFromIni(const std::string& iniPath);
    void CallCallbacks();
    
private: 
    using LineArg_Callback = std::function<void(const CommandLineArg&)>;
    using LineArg = std::string;

    std::unordered_map<LineArg, LineArg_Callback> m_CallbackMap;
    std::vector<CommandLineArg> m_Args;

private:
    inline void ResetReadBuffer() { memset(m_ReadBuffer, '\0', sizeof(m_ReadBuffer)); m_ReadBufferIndex = 0; }
    void CreateNewArg();
    void StartReadSubArgs();
    void NewSubArg();
    void EndParse();

    bool IsALegalChar(char l) const;

    bool m_ReadingSubArgs;
    bool m_IgnoreBackslash;
    char m_ReadBuffer[256];
    uint32_t m_ReadBufferIndex;
};

struct CommandLineArg
{
    std::string m_Arg;
    std::vector<std::string> m_SubArgs;
    bool Contains(const std::string& value);
};