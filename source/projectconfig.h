#pragma once

#define POP_LOGGING_ENABLED
#define POP_ASSERT_ENABLED
#define POP_PROFILE_ENABLED
#define POP_IMGUI_ENABLED

#define popSTRING(TEXT) #TEXT
#define popJOIN(A, B) A##B

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

using Byte = std::byte;

using String = std::string;

template<class T>
using Array = std::vector<T>;

template<class T, std::size_t N>
using InplaceArray = std::array<T, N>;

#define popObjectDynamicCast(ToType, ObjPtr) dynamic_cast<ToType*>(ObjPtr)
#define popObjectCast(ToType, ObjPtr) static_cast<ToType*>(ObjPtr)
