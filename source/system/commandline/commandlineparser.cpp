#include <system/precompile.h>

#include "system/commandline/commandlineparser.h"
#include "system/commandline/commandlineoptions.h"
#include <fstream>

#define POP_REGISTER_ARGUMENT(arg, descrption, callback)                                                 \
    m_CallbackMap.insert(std::make_pair<LineArg, LineArg_Callback>(arg, callback));


void CommandLineParser::RegisterCommandLineArgs()
{
    POP_REGISTER_ARGUMENT("/binarize", "/binarize:[outputpath, inputpath1, inputpath2 ...] - Calls serialize on Object", &CommandLineOptions::Binarize_Callback);
    POP_REGISTER_ARGUMENT("/fullscreen", "Launches Application in fullscreen", &CommandLineOptions::Fullscreen_Callback);
    POP_REGISTER_ARGUMENT("/showcursor", "Displays Cursor", &CommandLineOptions::ShowCursor_Callback);
    POP_REGISTER_ARGUMENT("/quit", "After Engine::Init we kill the process", &CommandLineOptions::QuitAfterInit_Callback);
    POP_REGISTER_ARGUMENT("/wireframe", "Draws meshes in wireframe", &CommandLineOptions::DrawWireframe_Callback);
}

#pragma region Parser Implementation

CommandLineParser::CommandLineParser() :
    m_ReadingSubArgs(false),
    m_IgnoreBackslash(false)
{
    ResetReadBuffer();
    RegisterCommandLineArgs();
}

void CommandLineParser::CallCallbacks()
{
    for (const CommandLineArg& cmdArg : m_Args)
    {
        auto it = m_CallbackMap.find(cmdArg.m_Arg);
        if (it != m_CallbackMap.end())
        {
            m_CallbackMap[cmdArg.m_Arg](cmdArg);
        }
    }
}

void CommandLineParser::ParseCommandLineArgsFromIni(const std::string& iniPath)
{
    std::ifstream reader;
    reader.open(iniPath, std::ifstream::in);
    if (!reader.is_open())
        return;

    std::vector<char> readBuffer;

    while (!reader.eof())
    {
        char l = '\0';
        reader >> l;
        if (l == '\n')
            continue;
        if (l == '=')
            break;
        if(IsALegalChar(l))
            readBuffer.push_back(l);
    }

    reader.close();

    uint32_t bufferSize = (uint32_t)readBuffer.size();
    char* charBuffer = new char[bufferSize + 1];
    for (uint32_t i = 0; i < bufferSize; ++i)
        charBuffer[i] = readBuffer[i];
    charBuffer[bufferSize] = '\0';

    ParseCommandLineArgs(charBuffer);
    delete[] charBuffer;
}

bool CommandLineParser::IsALegalChar(char l) const
{
    return (('a' <= l && l <= 'z') ||
            ('A' <= l && l <= 'Z') ||
            ('0' <= l && l <= '9') ||
            (l == ' ') ||
            (l == ':') ||
            (l == ',') ||
            (l == '.') ||
            (l == '/') ||
            (l == '\\') ||
            (l == '\"') ||
            (l == '\"'));
}

void CommandLineParser::ParseCommandLineArgs(char* args)
{
    if (strlen(args) == 0)
        return;

    popInfo(LogSystem, "Command Line Args : {}", args);

    char* currentLetter = args;
    while (true)
    {
        switch (*currentLetter)
        {
        case '\"': m_IgnoreBackslash = !m_IgnoreBackslash;  break; //Skip
        case ' ': break; //Skip
        case ':': StartReadSubArgs(); break;
        case ',': NewSubArg(); break;
        case '\0': 
            {
                EndParse(); 
                return;
            }
        case '/': CreateNewArg();
        default:
            {
                m_ReadBuffer[m_ReadBufferIndex] = *currentLetter;
                m_ReadBufferIndex++;
            }
        }
        currentLetter++;
    }
}

void CommandLineParser::CreateNewArg()
{
    if (m_IgnoreBackslash)
        return;

    if (!m_Args.empty() && !m_ReadingSubArgs && m_ReadBufferIndex != 0)
    {
        CommandLineArg& arg = m_Args[m_Args.size() - 1];
        arg.m_Arg = std::string(m_ReadBuffer);
    }
    else if (!m_Args.empty() && m_ReadingSubArgs && m_ReadBufferIndex != 0)
    {
        CommandLineArg& arg = m_Args[m_Args.size() - 1];
        arg.m_SubArgs.push_back(std::string(m_ReadBuffer));
    }

    ResetReadBuffer();
    CommandLineArg arg;
    m_Args.push_back(arg);
    m_ReadingSubArgs = false;
}

void CommandLineParser::StartReadSubArgs()
{
    CommandLineArg& arg = m_Args[m_Args.size() - 1];
    arg.m_Arg = std::string(m_ReadBuffer);
    ResetReadBuffer();

    m_ReadingSubArgs = true;
}

void CommandLineParser::EndParse()
{
    if (m_Args.size() > 0)
    {
        CommandLineArg& arg = m_Args[m_Args.size() - 1];
        if (m_ReadingSubArgs)
            arg.m_SubArgs.push_back(std::string(m_ReadBuffer));
        else
            arg.m_Arg = std::string(m_ReadBuffer);
    }

    ResetReadBuffer();
}

void CommandLineParser::NewSubArg()
{
    CommandLineArg& arg = m_Args[m_Args.size() - 1];
    arg.m_SubArgs.push_back(std::string(m_ReadBuffer));
    ResetReadBuffer();
}

#pragma endregion