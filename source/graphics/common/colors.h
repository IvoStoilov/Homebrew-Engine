#pragma once

class Color
{
public:
    constexpr Color(f32 r, f32 g, f32 b, f32 a) : m_RGBA(r, g, b, a) {}

    const f32* GetBuffer() const { return m_RGBA.GetBuffer(); }
private:
    vec4 m_RGBA;
};
namespace Colors
{
    static constexpr Color BLACK (0.f, 0.f, 0.f, 1.f);
    static constexpr Color WHITE (1.f, 1.f, 1.f, 1.f);

    static constexpr Color RED   (1.f, 0.f, 0.f, 1.f);
    static constexpr Color GREEN (0.f, 1.f, 0.f, 1.f);
    static constexpr Color BLUE  (0.f, 0.f, 1.f, 1.f);

    static constexpr Color PURPLE (1.f, 0.f, 1.f, 1.f);
    static constexpr Color TEAL   (0.f, 1.f, 1.f, 1.f);
    static constexpr Color YELLOW (1.f, 1.f, 0.f, 1.f);
}