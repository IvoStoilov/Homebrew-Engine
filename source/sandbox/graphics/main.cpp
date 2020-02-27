#include <system/precompile.h>

#include <system/math/vec.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>

using namespace std;

int main()
{
    CommandLineOptions::CreateInstance();
    ViewProvider::CreateInstance();
    g_ViewProvider.Update();
    ViewProvider::DestroyInstnace();
    vec4 a;
    vec4 b(1.f, 1.f, 1.f, 1.f);
    
    vec4 c = a + b;

    system("pause");
}