#include <system/precompile.h>
#include <system/viewprovider/windowcookie.h>
#include <system/viewprovider/viewprovider.h>

WindowCookie::WindowCookie() :
    m_Index(INVALID_INDEX)
{}

WindowCookie::WindowCookie(WindowCookie&& other) :
     m_Index(other.m_Index)
{
    other.m_Index = INVALID_INDEX;
}

WindowCookie& WindowCookie::operator=(WindowCookie&& other)
{
    m_Index = other.m_Index;
    other.m_Index = INVALID_INDEX;
    return *this;
}

WindowCookie::~WindowCookie()
{
    Clear();
}

void WindowCookie::Clear()
{
    g_ViewProvider.ClearChildWindow(*this);
    
}



