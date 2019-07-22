#include "system\error.h"
#include "windows.h"

void BreakWithMessageBox(const std::string& conditionString)
{
    MessageBox(nullptr, *(LPCWSTR*)(conditionString.c_str()), *(LPCWSTR*)("Assert Notification"), MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

    __debugbreak();
}