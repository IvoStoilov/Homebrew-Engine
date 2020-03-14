#pragma once
#include <extern/spdlog/include/spdlog/sinks/null_sink.h>
#include <extern/spdlog/include/spdlog/sinks/msvc_sink.h>
#include <extern/spdlog/include/spdlog/sinks/basic_file_sink.h>

#ifdef POP_LOGGING_ENABLED

#define POP_DECLARE_LOGSINK(logSink, sinkType)                                      \
public:                                                                             \
    inline const SharedPtr<sinkType>& Get##logSink() const { return m_##logSink; }  \
private:                                                                            \
    SharedPtr<sinkType> m_##logSink;

#define POP_IMPLEMENT_LOGSINK_CONSTRUCTOR_LIST(logSink, sinkType, ...)              \
    m_##logSink(std::make_shared<sinkType>(__VA_ARGS__))                            \

using LogSink = spdlog::sinks::sink;
class LogSinkDictionary
{
public:
    LogSinkDictionary(const String& fileName);

    POP_DECLARE_LOGSINK(NullSink, spdlog::sinks::null_sink_mt);
    POP_DECLARE_LOGSINK(MSVCSink, spdlog::sinks::msvc_sink_mt);
    POP_DECLARE_LOGSINK(FileSink, spdlog::sinks::basic_file_sink_mt);
};


#endif //POP_LOGGING_ENABLED