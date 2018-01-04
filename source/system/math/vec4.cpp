#include "system/math/vec4.h"
#include "system/math/mat4x4.h"
#include <string.h>

const vec4 vec4::Zero  (0.f, 0.f, 0.f, 0.f);
const vec4 vec4::BaseI (1.f, 0.f, 0.f, 0.f);
const vec4 vec4::BaseJ (0.f, 1.f, 0.f, 0.f);
const vec4 vec4::BaseK (0.f, 0.f, 1.f, 0.f);
const vec4 vec4::BaseW (0.f, 0.f, 0.f, 1.f);

vec4::vec4()
{
    memset(_data, 0, sizeof(_data));
}

vec4::vec4(float x, float y, float z, float w)
{
   _data[0] = x;
   _data[1] = y;
   _data[2] = z;
   _data[3] = w;
}

vec4::vec4(const vec4& value)
{
    memcpy(_data, value._data, sizeof(_data));
}

void vec4::operator= (const vec4& lhs)
{
   memcpy(_data, lhs._data, 4 * sizeof(float));
}

void vec4::operator+= (const vec4& lhs)
{
   _data[0] += lhs._data[0];
   _data[1] += lhs._data[1];
   _data[2] += lhs._data[2];
   _data[3] += lhs._data[3];
}

vec4 vec4::operator- (const vec4& lhs) const
{
   return vec4(_data[0] - lhs._data[0],
      _data[1] - lhs._data[1],
      _data[2] - lhs._data[2],
      _data[3] - lhs._data[3]);
}

vec4 vec4::operator+ (const vec4& lhs) const
{
   return vec4(_data[0] + lhs._data[0],
            _data[1] + lhs._data[1],
            _data[2] + lhs._data[2],
            _data[3] + lhs._data[3]);
}

vec4 vec4::operator/(const float lhs) const
{
   return vec4(_data[0] / lhs,
            _data[1] / lhs,
            _data[2] / lhs,
            _data[3] / lhs);
}

vec4 vec4::operator*(const float lhs) const
{
   return vec4(_data[0] * lhs,
            _data[1] * lhs,
            _data[2] * lhs,
            _data[3] * lhs);
}

vec4 vec4::operator*(const mat4x4& rhs) const
{
   mat4x4 transpose = rhs.GetTranspose();
   return vec4(vec4::Dot(*this, transpose[0]), 
              vec4::Dot(*this, transpose[1]),
            vec4::Dot(*this, transpose[2]),
            vec4::Dot(*this, transpose[3]));
}

void vec4::operator*= (const mat4x4& rhs)
{
   mat4x4 transpose = rhs.GetTranspose();
   float a = vec4::Dot(*this, transpose[0]);
   float b = vec4::Dot(*this, transpose[1]);
   float c = vec4::Dot(*this, transpose[2]);
   float d = vec4::Dot(*this, transpose[3]);
   _data[0] = a;
    _data[1] = b;
    _data[2] = c;
    _data[3] = d;
}

float& vec4::operator[] (uint32_t i)
{
   return _data[i];
}

float vec4::operator[](uint32_t i) const
{
   return _data[i];
}

float vec4::GetLengthSqr() const
{
   return vec4::Dot(*this, *this);
}

float vec4::GetLength() const
{
    return sqrt(GetLengthSqr());
}

void vec4::Normalize()
{
   float l = GetLength();
   _data[0] /= l;
   _data[1] /= l;
   _data[2] /= l;
   _data[3] /= l;
}

void vec4::Normalize2()
{
    float l = GetLength();
    _data[0] /= l;
    _data[1] /= l;
}

void vec4::Normalize3()
{
    float l = GetLength();
    _data[0] /= l;
    _data[1] /= l;
    _data[2] /= l;
}

vec4 vec4::GetNormalized() const
{
   float l = GetLength();
   return this->operator/(l);
}

void vec4::Print() const
{
   //std::printf("(%.2f, %.2f, %.2f, %.2f)\n", _data[0], _data[1], _data[2], _data[3]);
}

D3DXVECTOR4 vec4::ToD3DXVECTOR4() const
{
    return D3DXVECTOR4(_data[0], _data[1], _data[2], _data[3]);
}

float vec4::Dot(const vec4& a, const vec4& b)
{
   float res = 0.f;
   for (uint8_t i = 0; i < 4; ++i)
      res += a[i] * b[i];
   return res;
}

void vec4::Cross(const vec4&a, const vec4& b)
{
   //TODO : someday;
   //return Vec4::GetZero();
}