#include <system/precompile.h>

using namespace std;

int main()
{
    popInfo(LogSystem, "Hello {}", "ivO");
    popInfo(LogSystem, "Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    popCriticalError(false, LogSystem, "{:>8} aligned, {:<8} aligned", "right", "left");
}