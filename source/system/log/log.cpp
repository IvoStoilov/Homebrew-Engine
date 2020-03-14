#include <system/precompile.h>
#ifdef POP_LOGGING_ENABLED

#include <system/log/log.h>
#include <system/log/logsinkdictionary.h>

#include <extern/spdlog/include/spdlog/spdlog.h>
#include <extern/spdlog/include/spdlog/async.h>

constexpr size_t QUEUE_SIZE = 1024;
constexpr size_t THREAD_COUNT = 1;
static const String OUTPUT_FILE = "output.log";

Log::Log()
{
    spdlog::init_thread_pool(QUEUE_SIZE, THREAD_COUNT);
    
    m_Sinks = std::make_unique<LogSinkDictionary>(OUTPUT_FILE);
    m_Categories = std::make_unique<LogCategoryDictionary>(*m_Sinks);
}

Log::~Log()
{
    m_Categories.release();
    m_Sinks.release();
}

LogCategoryDictionary& Log::GetCategories()
{
    return *m_Categories;
}

#endif //POP_LOGGING_ENABLED