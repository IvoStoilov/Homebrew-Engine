#include <system/precompile.h>
#include <system/log/logsinkdictionary.h>

#ifdef POP_LOGGING_ENABLED

static constexpr bool TRUNCATE = true;
LogSinkDictionary::LogSinkDictionary(const String& fileName) :
    POP_IMPLEMENT_LOGSINK_CONSTRUCTOR_LIST(NullSink, spdlog::sinks::null_sink_mt),
    POP_IMPLEMENT_LOGSINK_CONSTRUCTOR_LIST(MSVCSink, spdlog::sinks::msvc_sink_mt),
    POP_IMPLEMENT_LOGSINK_CONSTRUCTOR_LIST(FileSink, spdlog::sinks::basic_file_sink_mt, fileName, TRUNCATE)
{}
#endif //POP_LOGGING_ENABLED