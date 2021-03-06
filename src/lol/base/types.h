//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#pragma once

#include <stdint.h>

namespace lol
{

/* It’s nice to have single-word type names, so define this. */
typedef long double ldouble;

/* The “real” type used for real numbers. It’s a specialisation of the
 * “Real” template class. */
template<typename T> class Real;
typedef Real<uint32_t> real;

/* The “half” type used for 16-bit floating point numbers. */
class half;

/*
 * Forward declaration of vec_t, mat_t, cmplx_t, quat_t, etc.
 */

/* HACK: if this is declared int const, Visual Studio becomes unable
 * to perform template argument deduction. */
#define FULL_SWIZZLE (0)

template<typename T, int N, int SWIZZLE = FULL_SWIZZLE> struct vec_t;
template<typename T, int N> struct box_t;
template<typename T, int COLS, int ROWS> struct mat_t;
template<typename T> struct cmplx_t;
template<typename T> struct quat_t;
template<typename T> struct dualquat_t;
template<typename T> struct sqt_t;

/*
 * Generic GLSL-like type names
 */

#define T_(tleft, tright, suffix) \
    typedef tleft half tright f16##suffix; \
    typedef tleft float tright suffix; \
    typedef tleft double tright d##suffix; \
    typedef tleft ldouble tright f128##suffix; \
    typedef tleft int8_t tright i8##suffix; \
    typedef tleft uint8_t tright u8##suffix; \
    typedef tleft int16_t tright i16##suffix; \
    typedef tleft uint16_t tright u16##suffix; \
    typedef tleft int32_t tright i##suffix; \
    typedef tleft uint32_t tright u##suffix; \
    typedef tleft int64_t tright i64##suffix; \
    typedef tleft uint64_t tright u64##suffix; \
    typedef tleft real tright r##suffix;

/* Idiotic hack to put "," inside a macro argument */
#define C_ ,

T_(vec_t<, C_ 2>, vec2)
T_(vec_t<, C_ 3>, vec3)
T_(vec_t<, C_ 4>, vec4)
T_(vec_t<, C_ 5>, vec5)
T_(vec_t<, C_ 6>, vec6)
T_(vec_t<, C_ 7>, vec7)
T_(vec_t<, C_ 8>, vec8)
T_(vec_t<, C_ 9>, vec9)
T_(vec_t<, C_ 10>, vec10)
T_(vec_t<, C_ 11>, vec11)
T_(vec_t<, C_ 12>, vec12)

T_(mat_t<, C_ 2 C_ 2>, mat2)
T_(mat_t<, C_ 3 C_ 3>, mat3)
T_(mat_t<, C_ 4 C_ 4>, mat4)

T_(mat_t<, C_ 2 C_ 3>, mat2x3)
T_(mat_t<, C_ 2 C_ 4>, mat2x4)
T_(mat_t<, C_ 3 C_ 2>, mat3x2)
T_(mat_t<, C_ 3 C_ 4>, mat3x4)
T_(mat_t<, C_ 4 C_ 2>, mat4x2)
T_(mat_t<, C_ 4 C_ 3>, mat4x3)

T_(cmplx_t<, >, cmplx)
T_(quat_t<, >, quat)
T_(dualquat_t<, >, dualquat)
T_(sqt_t<, >, sqt)

#undef C_
#undef T_

/*
 * HLSL/Cg-compliant type names
 */

typedef vec2 float2;
typedef vec3 float3;
typedef vec4 float4;
typedef vec5 float5;
typedef vec6 float6;
typedef vec7 float7;
typedef vec8 float8;
typedef vec9 float9;
typedef vec10 float10;
typedef vec11 float11;
typedef vec12 float12;
typedef mat2 float2x2;
typedef mat3 float3x3;
typedef mat4 float4x4;
typedef mat2x3 float2x3;
typedef mat2x4 float2x4;
typedef mat3x2 float3x2;
typedef mat3x4 float3x4;
typedef mat4x2 float4x2;
typedef mat4x3 float4x3;

typedef f16vec2 half2;
typedef f16vec3 half3;
typedef f16vec4 half4;
typedef f16mat2 half2x2;
typedef f16mat3 half3x3;
typedef f16mat4 half4x4;
typedef f16mat2x3 half2x3;
typedef f16mat2x4 half2x4;
typedef f16mat3x2 half3x2;
typedef f16mat3x4 half3x4;
typedef f16mat4x2 half4x2;
typedef f16mat4x3 half4x3;

typedef ivec2 int2;
typedef ivec3 int3;
typedef ivec4 int4;
typedef ivec5 int5;
typedef ivec6 int6;
typedef ivec7 int7;
typedef ivec8 int8;
typedef ivec9 int9;
typedef ivec10 int10;
typedef ivec11 int11;
typedef ivec12 int12;
typedef imat2 int2x2;
typedef imat3 int3x3;
typedef imat4 int4x4;
typedef imat2x3 int2x3;
typedef imat2x4 int2x4;
typedef imat3x2 int3x2;
typedef imat3x4 int3x4;
typedef imat4x2 int4x2;
typedef imat4x3 int4x3;

} /* namespace lol */

