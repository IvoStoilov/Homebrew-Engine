#include <system/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex.h>

using namespace std;

int main()
{
    GfxVertexLayout layout;
    layout.Append(GfxVertexPropertyType::Position3D);
    layout.Append(GfxVertexPropertyType::Normal);
    GfxVertexArray arr(layout);

    arr.EmplaceBack(vec3(0.f, 1.f, 0.f), vec3(0.f, 1.f, 0.f));
    cout << arr[0].GetProperty<GfxVertexPropertyType::Position3D>().y;
}