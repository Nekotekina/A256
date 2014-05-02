#pragma once

#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <algorithm>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <memory.h>
#include <limits>
#include <climits>
#include <cfloat>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;
typedef signed char f8;
typedef signed short f16;
typedef float f32;
typedef double f64;
typedef __m128i u128;
typedef __m256i u256;

namespace fmt
{
	std::string format(const std::string& fmt, ...)
	{
		size_t size = 256;
		std::string res;
		va_list v;
		while (true)
		{
			res.resize(size);
			va_start(v, fmt);
			int count = vsnprintf_s(&res[0], size, size, fmt.c_str(), v);
			va_end(v);
			if (count >= 0 && count < size)
			{
				res.resize(count);
				return res;
			}
			if (count >= 0)
			{
				size = count + 1;
			}
			else
			{
				size = size * 2;
			}	
		}
		return res;
	}
};