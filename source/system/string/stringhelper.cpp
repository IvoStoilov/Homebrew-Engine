#include <system/precompile.h>
#include <system/string/stringhelper.h>

String StringHelper::FormatTime(const std::chrono::system_clock::time_point& timePoint)
{
    std::time_t t = std::chrono::system_clock::to_time_t(timePoint);

    String result(30, '\0');
    std::strftime(&result[0], result.size(), "%Y-%m-%d_%H-%M-%S", std::localtime(&t));
    return result;
}
