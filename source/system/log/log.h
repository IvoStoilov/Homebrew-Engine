#pragma once
#include <system/log/logcategory.h>

/*
String Patterns :
"Hello, {}!", "World"
"Easy padding in numbers like {:08d}", 12;
"Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42;
"Support for floats {:03.2f}", 1.23456;
"Positional args are {1} {0}..", "too", "supported";
"{:>8} aligned, {:<8} aligned", "right", "left";
*/

#define popLogCategories Log::GetInstance().GetCategories()

class Log
{
public:
    static Log& GetInstance() { static Log ms_Instance; return ms_Instance; }

    Log();
    ~Log();

    LogCategoryDictionary& GetCategories();
private:
    UniquePtr<LogSinkDictionary> m_Sinks;
    UniquePtr<LogCategoryDictionary> m_Categories;

};

#ifdef POP_LOGGING_ENABLED
#define popTrace(logCategory, ...)                    popLogCategories.Get##logCategory()->trace(__VA_ARGS__)
#define popDebug(logCategory, ...)                    popLogCategories.Get##logCategory()->debug(__VA_ARGS__)
#define popInfo(logCategory, ...)                     popLogCategories.Get##logCategory()->info (__VA_ARGS__)
#define popWarning(condition, logCategory, ...)       if(!(condition)) { popLogCategories.Get##logCategory()->warn    (__VA_ARGS__); }
#define popError(condition, logCategory, ...)         if(!(condition)) { popLogCategories.Get##logCategory()->error   (__VA_ARGS__); }
#define popCriticalError(condition, logCategory, ...) if(!(condition)) { popLogCategories.Get##logCategory()->critical(__VA_ARGS__); }
#else
#define popTrace(logCategory, ...)
#define popDebug(logCategory, ...)
#define popInfo(logCategory, ...)
#define popWarning(logCategory, ...)
#define popError(logCategory, ...)
#define popCriticalError(logCategory, ...)
#endif //POP_LOGGING_ENABLED