#include <system/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertex.h>

using namespace std;

int main()
{
    GfxVertexLayout layout;
    layout.Append(GfxVertexPropertyType::Position3D);
    layout.Append(GfxVertexPropertyType::Normal);
    GfxVertexArray arr(layout);
}