#pragma once
//Graphics
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <dxgidebug.h>
#include <d3d11.h>
#include <wrl.h>

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

#include <projectconfig.h>

#include <system/error.h>
#include <system/log/log.h>
#include <system/profiling/profilemanager.h>
#include <system/math/vec.h>
#include <system/math/mat4x4.h>
#include <system/math/mathutil.h>

#include <graphics/gfxinfoqueue.h>