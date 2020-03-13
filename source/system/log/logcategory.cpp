#include <system/precompile.h>
#include <system/log/logcategory.h>

#ifdef POP_LOGGING_ENABLED

#include <system/log/logsinkdictionary.h>

#include <extern/spdlog/include/spdlog/spdlog.h>

LogCategoryDictionary::LogCategoryDictionary(const LogSinkDictionary& sinkDictionary)
{
    POP_IMPLEMENT_LOGCATEGORY(LogSystem, sinkDictionary);
    POP_IMPLEMENT_LOGCATEGORY(LogGraphics, sinkDictionary);
    POP_IMPLEMENT_LOGCATEGORY(LogEngine, sinkDictionary);
}
#endif //POP_LOGGING_ENABLED


