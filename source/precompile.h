#pragma once
#include <memory>
#include <stdint.h>
#include <string>

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T>
using SharedPtr = std::shared_ptr<T>;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;

using String = std::string;

#define popObjectDynamicCast(ToType, ObjPtr) dynamic_cast<ToType*>(ObjPtr)
#define popObjectCast(ToType, ObjPtr) static_cast<ToType*>(ObjPtr)
