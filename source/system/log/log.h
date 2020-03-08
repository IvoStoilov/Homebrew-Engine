#pragma once
#ifdef POP_LOGGING_ENABLED
#include <extern/spdlog/include/spdlog/spdlog.h>
class Log
{
public:
    static void CreateInstance();
    static void CleanInstance();
    static Log* GetInstance();

    bool Initialize();
    void Shutdown();
private:
    static Log* ms_Instance;
    SharedPtr<spdlog::logger> m_Logger;
};
#endif //POP_LOGGING_ENABLED