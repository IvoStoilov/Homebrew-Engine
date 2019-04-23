#pragma once
#include "system/math/vec4.h"
#include "system/math/mathutil.h"
#include <d3d11.h>
//todo remove d3dx10math
#include <d3dx10math.h>

#include <directxmath.h>

__declspec(align(16)) class mat4x4
{
protected:
    vec4 _data[4];

public:
    static mat4x4 Identity;

public:
    mat4x4();
    mat4x4(const vec4& rowI, const vec4& rowJ, const vec4& rowK, const vec4& rowW);
    mat4x4(const mat4x4& value);
    mat4x4(const D3DXMATRIX& value);

    inline void SetRowI(const vec4& v) { _data[0] = v; }
    inline void SetRowJ(const vec4& v) { _data[1] = v; }
    inline void SetRowK(const vec4& v) { _data[2] = v; }
    inline void SetRowW(const vec4& v) { _data[3] = v; }

    inline vec4 GetRowI() const { return _data[0]; };
    inline vec4 GetRowJ() const { return _data[1]; };
    inline vec4 GetRowK() const { return _data[2]; };
    inline vec4 GetRowW() const { return _data[3]; };

    void operator=(const mat4x4& value);
    void operator=(const D3DXMATRIX& value);

    inline const vec4 operator[] (const uint32_t row) const;
    //inline vec4& operator[] (const uint32_t row);

    mat4x4 GetTranspose() const;
    static void Tranpose(mat4x4& mat);

    mat4x4 operator*(mat4x4& rhs);
    void   operator*=(const mat4x4& rhs);

    void   operator+=(const mat4x4& rhs);
    
    mat4x4 operator*(const vec4& rhs);

    //Deprecate
    D3DXMATRIX ToD3DXMATRIX() { return D3DXMATRIX(_data[0][0], _data[0][1], _data[0][2], _data[0][3], 
                                                  _data[1][0], _data[1][1], _data[1][2], _data[1][3], 
                                                  _data[2][0], _data[2][1], _data[2][2], _data[2][3], 
                                                  _data[3][0], _data[3][1], _data[3][2], _data[3][3]); }

    DirectX::XMMATRIX ToXMMATRIX() { return DirectX::XMMATRIX(_data[0][0], _data[0][1], _data[0][2], _data[0][3],
                                                              _data[1][0], _data[1][1], _data[1][2], _data[1][3],
                                                              _data[2][0], _data[2][1], _data[2][2], _data[2][3],
                                                              _data[3][0], _data[3][1], _data[3][2], _data[3][3]); }

    static inline mat4x4 GetRotateXAxisMatrix(float rads)
    {
        return mat4x4(vec4(1.f, 0.f	     , 0.f       , 0.f),
                      vec4(0.f, cos(rads), -sin(rads), 0.f),
                      vec4(0.f, sin(rads),  cos(rads), 0.f),
                      vec4(0.f, 0.f      , 0.f       , 1.f));
    }

    static inline mat4x4 GetRotateYAxisMatrix(float rads)
    {
        return mat4x4(vec4(+cos(rads), 0.f, +sin(rads), 0.f),
                      vec4(       0.f, 1.f,        0.f, 0.f),
                      vec4(-sin(rads), 0.f, +cos(rads), 0.f),
                      vec4(		  0.f, 0.f,        0.f, 1.f));
    }

    static inline mat4x4 GetRotateZAxisMatrix(float rads)
    {
        return mat4x4(vec4(+cos(rads), -sin(rads), 0.f, 0.f),
                      vec4(+sin(rads), +cos(rads), 0.f, 0.f),
                      vec4(		  0.f,		  0.f, 1.f, 0.f),
                      vec4(	      0.f,        0.f, 0.f, 1.f));
    }

    static inline mat4x4 GetRotateMatrix(float theta, const vec4& axis)
    {
        vec4 u = axis.GetNormalized();
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        return mat4x4(vec4(cosTheta + u[0] * u[0] * (1.f - cosTheta), u[0] * u[1] * (1 - cosTheta) - u[2] * sinTheta, u[0] * u[2] * (1 - cosTheta) + u[1] * sinTheta, 0.f),
                      vec4(u[1] * u[0] * (1 - cosTheta) + u[2] * sinTheta, cosTheta + u[1] * u[1] * (1 - cosTheta), u[1] * u[2] * (1 - cosTheta) - u[0] * sinTheta, 0.f),
                      vec4(u[2] * u[0] * (1 - cosTheta) - u[1] * sinTheta, u[2] * u[1] * (1 - cosTheta) + u[0] * sinTheta, cosTheta + u[2] * u[2] * (1 - cosTheta), 0.f),
                      vec4(0.f, 0.f, 0.f, 1.f));
    }
};

class Transform : public mat4x4
{
public:
    Transform();
    Transform(const mat4x4& value);

    inline void SetTranslate(const vec4& translate) { _data[3] = translate; }
    inline vec4 GetTranslate() const { return _data[3]; }

    mat4x4 GetRotation() const;
    void SetRotation(const mat4x4& rotation);

    void NormalizeCollums();
};