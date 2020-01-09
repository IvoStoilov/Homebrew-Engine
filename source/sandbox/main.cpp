#include <iostream>
#include <system/math/vec4.h>

using namespace std;

int main()
{
    vec4 a(0.f, 0.f, 0.f, 0.f);
    vec4 b(2.f, 1.f, 1.f, 0.f);

    vec4 c = a + b;
    cout << c.GetLength() << endl;
    cout << "hello world" << endl;
    system("pause");
    return 0;
}