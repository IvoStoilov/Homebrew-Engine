#include <system/precompile.h>
#include "system\error.h"
#include "windows.h"

void BreakWithMessageBox(const std::string& conditionString)
{
    MessageBox(nullptr, conditionString.c_str(), "Assert Notification", MB_OKCANCEL | MB_ICONEXCLAMATION | MB_SETFOREGROUND);

    __debugbreak();
}