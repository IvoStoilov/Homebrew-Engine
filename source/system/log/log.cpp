#include <system/precompile.h>
#ifdef POP_LOGGING_ENABLED
#include <system/log/log.h>
#include <system/log/logcategory.h>

#include <extern/spdlog/include/spdlog/sinks/basic_file_sink.h>
#include <extern/spdlog/include/spdlog/sinks/msvc_sink.h>
#include <extern/spdlog/include/spdlog/async.h>

#include <fstream>

bool Log::Initialize()
{
    return Log::Initialize("output.log", LogCategory::CATEGORIES);
}

bool Log::Initialize(const String& outputFile, const Array<String>& categoryNames)
{
    constexpr u32 QUEUE_SIZE = 8192;
    constexpr u32 THREAD_COUNT = 1;

    spdlog::init_thread_pool(QUEUE_SIZE, THREAD_COUNT);
    spdlog::set_pattern("%^[%T] %n: %v%$");

    ClearOutputFile(outputFile);
    Array<SharedPtr<LogSink>> logSinks;
    InitializeSinks(outputFile, logSinks);
    InitializeCategories(categoryNames, logSinks);

    return true;
}

void Log::Shutdown()
{
    spdlog::shutdown();
}

void Log::ClearOutputFile(const String& outputFile)
{
    std::ofstream ofs;
    ofs.open(outputFile, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

void Log::InitializeSinks(const String& outputFile, Array<SharedPtr<LogSink>>& outLogSinks)
{
    outLogSinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
    outLogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(outputFile));
}

void Log::InitializeCategories(const Array<String>& categoryNames, const Array<SharedPtr<LogSink>>& logSinks)
{
    for (const String& name : categoryNames)
    {
        auto logger = std::make_shared<spdlog::async_logger>(name, logSinks.begin(), logSinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        logger->set_level(spdlog::level::trace);
        spdlog::register_logger(logger);
    }
}

#endif //POP_LOGGING_ENABLED