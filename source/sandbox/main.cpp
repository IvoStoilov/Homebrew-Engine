#include <system/precompile.h>

#include <system/math/vec4.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>
#include <extern/DirectXTK/Inc/SimpleMath.h>

using namespace std;

int main()
{
    CommandLineOptions::CreateInstance();
    ViewProvider::CreateInstance();
    g_ViewProvider.Update();
    ViewProvider::DestroyInstnace();
    DirectX::SimpleMath::Vector4 a;
    DirectX::SimpleMath::Vector4 b(1.f, 1.f, 1.f, 1.f);

    DirectX::SimpleMath::Vector4 c = a + b;

    system("pause");
}