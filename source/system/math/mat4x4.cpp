#include "system/math/mat4x4.h"
#include <string.h>
#include <algorithm>
#include <xmmintrin.h>


mat4x4 mat4x4::Identity = mat4x4(vec4(1.f, 0.f, 0.f, 0.f),
                                 vec4(0.f, 1.f, 0.f, 0.f),
                                 vec4(0.f, 0.f, 1.f, 0.f),
                                 vec4(0.f, 0.f, 0.f, 1.f));


mat4x4::mat4x4()
{
    _data[0] = vec4::BaseI;
    _data[1] = vec4::BaseJ;
    _data[2] = vec4::BaseK;
    _data[3] = vec4::BaseW;
}

mat4x4::mat4x4(const mat4x4& value)
{
    _data[0] = value[0];
    _data[1] = value[1];
    _data[2] = value[2];
    _data[3] = value[3];
}

mat4x4::mat4x4(const vec4& i, const vec4& j, const vec4& k, const vec4& w)
{
    _data[0] = i;
    _data[1] = j;
    _data[2] = k;
    _data[3] = w;
}

////vec4& mat4x4::operator[](const uint32_t row)
////{
////	return _data[row];
////}

const vec4 mat4x4::operator[](const uint32_t row) const
{
    return _data[row];
}

void mat4x4::operator=(const mat4x4& rhs)
{
    memcpy(this->_data, rhs._data, sizeof(mat4x4));
}


void mat4x4::operator+=(const mat4x4&rhs)
{
    _data[0] += rhs._data[0];
    _data[1] += rhs._data[1];
    _data[2] += rhs._data[2];
    _data[3] += rhs._data[3];
}

void mat4x4::operator*= (const mat4x4& rhs)
{
    __m128 row1 = _mm_load_ps(rhs._data[0]._data);
    __m128 row2 = _mm_load_ps(rhs._data[1]._data);
    __m128 row3 = _mm_load_ps(rhs._data[2]._data);
    __m128 row4 = _mm_load_ps(rhs._data[3]._data);

    for (int i = 0; i < 4; ++i)
    {
        __m128 brod1 = _mm_set1_ps(_data[0]._data[i]);
        __m128 brod2 = _mm_set1_ps(_data[1]._data[i]);
        __m128 brod3 = _mm_set1_ps(_data[2]._data[i]);
        __m128 brod4 = _mm_set1_ps(_data[3]._data[i]);

        __m128 row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(brod1, row1),
            _mm_mul_ps(brod2, row2)),
            _mm_add_ps(_mm_mul_ps(brod3, row3),
            _mm_mul_ps(brod4, row4)));

        _mm_store_ps(_data[i]._data, row);
    }
}

mat4x4 mat4x4::GetTranspose() const
{
    return mat4x4(vec4(_data[0][0], _data[1][0], _data[2][0], _data[3][0]),
                  vec4(_data[0][1], _data[1][1], _data[2][1], _data[3][1]),
                  vec4(_data[0][2], _data[1][2], _data[2][2], _data[3][2]),
                  vec4(_data[0][3], _data[1][3], _data[2][3], _data[3][3]));
}

void mat4x4::Tranpose(mat4x4& mat)
{
    for (uint8_t i = 0; i < 4; ++i)
    {
        for (uint8_t j = 0; j < 4; ++j)
        {
            std::swap(mat._data[i][j], mat._data[j][i]);
        }
    }
}

mat4x4 mat4x4::operator*(mat4x4& lhs)
{
    mat4x4 result;
    __m128 row1 = _mm_load_ps(lhs._data[0]._data);
    __m128 row2 = _mm_load_ps(lhs._data[1]._data);
    __m128 row3 = _mm_load_ps(lhs._data[2]._data);
    __m128 row4 = _mm_load_ps(lhs._data[3]._data);

    for (int i = 0; i < 4; ++i)
    {
        __m128 brod1 = _mm_set1_ps(_data[0]._data[i]);
        __m128 brod2 = _mm_set1_ps(_data[1]._data[i]);
        __m128 brod3 = _mm_set1_ps(_data[2]._data[i]);
        __m128 brod4 = _mm_set1_ps(_data[3]._data[i]);

        __m128 row = _mm_add_ps(_mm_add_ps(_mm_mul_ps(brod1, row1),
                                           _mm_mul_ps(brod2, row2)),
                                _mm_add_ps(_mm_mul_ps(brod3, row3),
                                           _mm_mul_ps(brod4, row4)));

        _mm_store_ps(result._data[i]._data, row);
    }

    return result;

}


Transform::Transform(const mat4x4& mat) :
    mat4x4(mat)
{
}

Transform::Transform() :
    mat4x4()
{
}

mat4x4 Transform::GetRotation() const
{
    return mat4x4(_data[0], _data[1], _data[2], vec4::BaseW);
}

void Transform::SetRotation(const mat4x4& rotation)
{
    _data[0] = rotation.GetRowI();
    _data[1] = rotation.GetRowJ();
    _data[2] = rotation.GetRowK();
}

void Transform::NormalizeCollums()
{
    for (uint8_t i = 0; i < 3; ++i)
    {
        //vec4 collum(_data[0][i], _data[1][i], _data[2][i], 0);
        //collum.Normalize();
        //_data[0][i] = collum._data[0];
        //_data[1][i] = collum._data[1];
        //_data[2][i] = collum._data[2];
        
        _data[i].Normalize();
    }

    _data[3][3] = 1;
}