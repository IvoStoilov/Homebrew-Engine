#pragma once
#ifdef POP_LOGGING_ENABLED
#include <extern/spdlog/include/spdlog/spdlog.h>

/*
String Patterns :
"Hello, {}!", "World"
"Easy padding in numbers like {:08d}", 12;
"Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42;
"Support for floats {:03.2f}", 1.23456;
"Positional args are {1} {0}..", "too", "supported";
"{:>8} aligned, {:<8} aligned", "right", "left";
*/

class Log
{
public:
    static bool Initialize();
    static bool Initialize(const String& outputFile, const Array<String>& categoryNames);
    static void Shutdown();

private:
    using LogSink = spdlog::sinks::sink;
    static void ClearOutputFile(const String& outputFile);
    static void InitializeSinks(const String& outputFile, Array<SharedPtr<LogSink>>& outLogSinks);
    static void InitializeCategories(const Array<String>& categoryNames, const Array<SharedPtr<LogSink>>& logSinks);
};

#endif //POP_LOGGING_ENABLED

#ifdef POP_LOGGING_ENABLED
#define popTrace(logCategory, ...) spdlog::get(STRING(logCategory))->trace(__VA_ARGS__)
#define popDebug(logCategory, ...) spdlog::get(STRING(logCategory))->debug(__VA_ARGS__)
#define popInfo(logCategory, ...) spdlog::get(STRING(logCategory))->info (__VA_ARGS__)
#define popWarning(condition, logCategory, ...) if(!(condition)) { spdlog::get(STRING(logCategory))->warn    (__VA_ARGS__); }
#define popError(condition, logCategory, ...) if(!(condition)) { spdlog::get(STRING(logCategory))->error   (__VA_ARGS__); }
#define popCriticalError(condition, logCategory, ...) if(!(condition)) { spdlog::get(STRING(logCategory))->critical(__VA_ARGS__); }
#else
#define popTrace
#define popDebug
#define popInfo
#define popWarning
#define popError 
#define popCriticalError
#endif //POP_LOGGING_ENABLED