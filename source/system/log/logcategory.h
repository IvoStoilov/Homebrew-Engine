#pragma once
#ifdef POP_LOGGING_ENABLED
using AsyncLogger = spdlog::async_logger;

#define POP_DECLARE_LOGCATEGORY(logCategory)                                           \
public:                                                                                \
    const SharedPtr<AsyncLogger>& Get##logCategory() const { return m_##logCategory; } \
private:                                                                               \
    SharedPtr<AsyncLogger> m_##logCategory;

#define POP_SINK_LIST(sinkDictionary) spdlog::sinks_init_list{ sinkDictionary.GetMSVCSink(), sinkDictionary.GetFileSink()}

#define POP_IMPLEMENT_LOGCATEGORY(logCategory, sinkDictionary)                                                                    \
    m_##logCategory = std::make_shared<AsyncLogger>(popSTRING(logCategory), POP_SINK_LIST(sinkDictionary), spdlog::thread_pool());\
    m_##logCategory->set_pattern("[%H:%M:%S.%e][%n] %v");                                                                           \
    m_##logCategory->set_level(spdlog::level::trace);

#define popLogCategoryDictionary LogCategoryDictionary::GetInstance()

class LogSinkDictionary;
class LogCategoryDictionary
{
public:
    LogCategoryDictionary(const LogSinkDictionary& sinkDictionary);

private:
    POP_DECLARE_LOGCATEGORY(LogSystem);
    POP_DECLARE_LOGCATEGORY(LogGraphics);
    POP_DECLARE_LOGCATEGORY(LogEngine);

};

#endif //POP_LOGGING_ENABLED
