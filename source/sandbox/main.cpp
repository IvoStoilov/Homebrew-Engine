#include <system/precompile.h>
#include <iostream>
#include <system/math/vec4.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>

using namespace std;

int main()
{
    CommandLineOptions::CreateInstance();
    ViewProvider::CreateInstance();
    while (true)
    {

    }
    ViewProvider::DestroyInstnace();
}