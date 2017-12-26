#pragma once
#include <stdint.h>
#include <math.h>

class mat4x4;

class vec4
{
public:
   vec4();
   vec4(const vec4& value);
   vec4(float x, float y, float z, float w);
   
   void operator=  (const vec4& rhs);
   void operator+= (const vec4& rhs);

   vec4 operator+ (const vec4& rhs) const;
   vec4 operator- (const vec4& rhs) const;

   vec4 operator/ (const float rhs) const;
   vec4 operator* (const float rhs) const;
   vec4 operator* (const mat4x4& rhs) const;
   void operator*=(const mat4x4& rhs);


   float& operator[] (uint32_t i);
   float operator[](uint32_t i) const;

   float GetLengthSqr() const;
   float GetLength() const;

   void Normalize();
   void Normalize2();
   void Normalize3();
   vec4 GetNormalized() const;
   
   void Print() const;

   static float Dot(const vec4& a, const vec4& b);
   static void Cross(const vec4&a, const vec4& b);

public:
   float _data[4];

public:
   static const vec4 Zero;
   static const vec4 BaseI;
   static const vec4 BaseJ;
   static const vec4 BaseK;
   static const vec4 BaseW;
};