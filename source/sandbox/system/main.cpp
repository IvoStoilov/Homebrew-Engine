#include <system/precompile.h>
#include <system/log/log.h>
using namespace std;

int main()
{
    Log::Initialize();

    popDebug(logSystem, "Hello {}", "ivO");
    popInfo(logSystem, "Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    popCriticalError(false, logSystem, "{:>8} aligned, {:<8} aligned", "right", "left");
    system("pause");
    Log::Shutdown();
}