#include "system/math/vec2.h"
#include "system/error.h"

#include <math.h>
#include <string.h>

#include <d3d11.h>
#include <d3dx10math.h>

const vec2 vec2::Zero  (0.f, 0.f);
const vec2 vec2::BaseI (1.f, 0.f);
const vec2 vec2::BaseJ (0.f, 1.f);

vec2::vec2()
{
    memset(_data, 0, sizeof(_data));
}

vec2::vec2(const vec2& value)
{
    memcpy(_data, value._data, sizeof(_data));
}

vec2::vec2(float x, float y)
{
    _data[0] = x;
    _data[1] = y;
}

void vec2::operator= (const vec2& rhs)
{
    memcpy(_data, rhs._data, sizeof(_data));
}

void vec2::operator+= (const vec2& rhs)
{
    _data[0] += rhs._data[0];
    _data[1] += rhs._data[1];
}

void vec2::operator-= (const vec2& rhs)
{
    _data[0] -= rhs._data[0];
    _data[1] -= rhs._data[1];
}

void vec2::operator*= (const float rhs)
{
    _data[0] *= rhs;
    _data[1] *= rhs;
}

void vec2::operator/= (const float rhs)
{
    popAssert(rhs != 0, "vec2::operator/= - Division by 0");
    _data[0] /= rhs;
    _data[1] /= rhs;
}

vec2 vec2::operator+ (const vec2& rhs) const
{
    return vec2(_data[0] + rhs._data[0], _data[1] + rhs._data[1]);
}

vec2 vec2::operator- (const vec2& rhs) const
{
    return vec2(_data[0] - rhs._data[0], _data[1] - rhs._data[1]);
}

vec2 vec2::operator* (const float rhs) const
{
    return vec2(_data[0] * rhs , _data[1] * rhs);
}

vec2 vec2::operator/ (const float rhs) const
{
    popAssert(rhs != 0, "vec2::operator/ - Division by 0");
    return vec2(_data[0] / rhs, _data[1] / rhs);
}

float& vec2::operator[] (const uint32_t i)
{
    popAssert(i < 2, "vec2 - Out of vector bounds");
    return _data[i];
}

float vec2::operator[] (const uint32_t i) const
{
    popAssert(i < 2, "vec2 - Out of vector bounds");
    return _data[i];
}

float vec2::GetLengthSqr() const
{
    return vec2::Dot(*this, *this);
}

float vec2::GetLength() const
{
    return sqrtf(GetLengthSqr());
}

void vec2::Normalize()
{
    float l = GetLength();
    this->operator/=(l);
}

vec2 vec2::GetNormalized() const
{
    vec2 result(*this);
    result.Normalize();
    return result;
}

D3DXVECTOR2 vec2::ToD3DXVECTOR2() const
{
    return D3DXVECTOR2(_data[0], _data[1]);
}

float vec2::Dot(const vec2& a, const vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

