#pragma once

class StringHelper
{
public:
    template <typename ... Args>
    static String Format(const String& format, Args... args);
    static String FormatTime(const std::chrono::system_clock::time_point& timePoint);
};

template <typename ... Args>
String StringHelper::Format(const String& format, Args... args)
{
    u32 size = static_cast<u32>(snprintf(nullptr, 0, format.c_str(), args...)) + 1;
    if (format.size() > 0 && size == 0)
    {
        popAssert(size > 0, "Failed to format string: {}", format);
        return format;
    }

    UniquePtr<char> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return String(buf.get(), buf.get() + size - 1);
}