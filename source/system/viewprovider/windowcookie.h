#pragma once

class WindowCookie
{
public:
    WindowCookie();
    WindowCookie(WindowCookie&& other);
    WindowCookie& operator=(WindowCookie&& other);
    ~WindowCookie();

    WindowCookie(const WindowCookie&) = delete;
    WindowCookie& operator= (const WindowCookie&) = delete;

    void Clear();

private:
    explicit WindowCookie(u32 id) : m_Index(id) {}

    static constexpr u32 INVALID_INDEX = 0xFFFFFFFF;
    u32 m_Index = INVALID_INDEX;

    friend class ViewProvider;
};