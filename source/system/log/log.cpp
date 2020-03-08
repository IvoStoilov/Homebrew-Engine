#include <system/precompile.h>
#ifdef POP_LOGGING_ENABLED
#include <system/log/log.h>

#include <extern/spdlog/include/spdlog/sinks/basic_file_sink.h>

Log* Log::ms_Instance = nullptr;

void Log::CreateInstance()
{
    if (!ms_Instance)
    {
        ms_Instance = new Log();
        ms_Instance->Initialize();
    }
}


void Log::CleanInstance()
{
    if (ms_Instance)
    {
        ms_Instance->CleanInstance();
        delete ms_Instance;
    }
}

Log* Log::GetInstance()
{
    popAssert(ms_Instance, "Log::CreateInstance was not called yet.");
    return ms_Instance;
}

bool Log::Initialize()
{
    m_Logger = spdlog::basic_logger_mt("main logger", "homebrew_output.txt");
}

void Log::Shutdown()
{

}
#endif //POP_LOGGING_ENABLED