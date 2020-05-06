#include <system/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex/gfxvertexarray.h>

using namespace std;

int main()
{
    GfxVertexLayout layout;
    layout.Append(GfxVertexPropertyType::Position3D);
    layout.Append(GfxVertexPropertyType::Normal);
    GfxVertexArray arr(layout);

    arr.EmplaceBack(vec3(0.f, 1.f, 0.f), vec3(0.f, 1.f, 1.f));
    vec3& normal = arr[0].GetProperty<GfxVertexPropertyType::Normal>();
    normal.x += 2.f;
    vec3& newNormal = arr[0].GetProperty<GfxVertexPropertyType::Normal>();
    arr[0].SetProperty<GfxVertexPropertyType::Position3D>(vec3(4, 4, 4));
    vec3 pos = arr[0].GetProperty<GfxVertexPropertyType::Position3D>();
    cout << arr[0].GetProperty<GfxVertexPropertyType::Position3D>().y;
}