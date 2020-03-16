#include <system/precompile.h>

using namespace std;

void CoutProfile()
{
    popProfile(CoutProfile)
    for (int i = 0; i < 1000; i++)
    {
        cout << i << endl;
    }
}

void PrintFProfile()
{
    popProfile(PrintFProfile)
    for (int i = 0; i < 1000; i++)
    {
        printf("Hello");
    }
}

int main()
{
    ProfileManager::CreateInstance();
    g_ProfileManager.BeginSession();
    {
        popProfile(Frame)
        {
            popProfile(Logging)
                popInfo(LogSystem, "Hello {}", "ivO");
            popInfo(LogSystem, "Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
            popCriticalError(false, LogSystem, "{:>8} aligned, {:<8} aligned", "right", "left");
        }
        CoutProfile();
        PrintFProfile();
    }

    g_ProfileManager.EndSession();
}