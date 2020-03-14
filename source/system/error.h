#pragma once
#include <extern/spdlog/include/spdlog/fmt/bundled/core.h>
#include <extern/spdlog/include/spdlog/fmt/bundled/format.h>

#ifdef POP_ASSERT_ENABLED
    #define popAssert(condition, conditionString, ...)         \
        if(!(condition))                                       \
        {                                                      \
            BreakWithMessageBox(conditionString, __VA_ARGS__); \
        }                                        
#else
    #define popAssert(condition, conditionString, ...)         \
        condition
#endif //POP_ASSERT_ENABLED

template <typename... Args>
void BreakWithMessageBox(const char* conditionString, const Args &... args)
{
    constexpr u32 MAX_SIZE = 1024;
    char buffer[MAX_SIZE];
    memset(buffer, 0, MAX_SIZE);
    fmt::format_to(buffer, conditionString, args...);
    MessageBox(nullptr, buffer, "Assert Notification", MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND);
    
    __debugbreak();
}
