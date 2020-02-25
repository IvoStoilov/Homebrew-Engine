#pragma once
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <d3d11.h>

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T>
using SharedPtr = std::shared_ptr<T>;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using f32 = float;
using f64 = double;
using f128 = long double;

using String = std::string;

template<class T>
using Array = std::vector<T>;

#define popObjectDynamicCast(ToType, ObjPtr) dynamic_cast<ToType*>(ObjPtr)
#define popObjectCast(ToType, ObjPtr) static_cast<ToType*>(ObjPtr)

#include <system/math/vec.h>
#include <system/math/mat4x4.h>
