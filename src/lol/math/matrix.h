//
//  Lol Engine
//
//  Copyright © 2010—2019 Sam Hocevar <sam@hocevar.net>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

//
// The matrix classes
// ------------------
//

#include <ostream>

#include <lol/math/vector.h>
#include <lol/math/transform.h>

#if _WIN32
#   pragma push_macro("near")
#   pragma push_macro("far")
#   undef near
#   undef far
#endif

namespace lol
{

/*
 * The generic “mat_t” type, which is fixed-size
 */

template<typename T, int COLS, int ROWS>
struct LOL_ATTR_NODISCARD mat_t
  : public linear_ops::base<vec_t<T,ROWS>>
{
    static int const count = COLS;
    typedef T scalar_element;
    typedef vec_t<T,ROWS> element;
    typedef mat_t<T,COLS,ROWS> type;

    inline mat_t() {}

    explicit inline mat_t(T const &val)
    {
        T const zero = T(0);
        for (int i = 0; i < COLS; ++i)
            for (int j = 0; j < ROWS; ++j)
                m_data[i][j] = i == j ? val : zero;
    }

    /* Explicit constructor for type conversion */
    template<typename U>
    explicit inline mat_t(mat_t<U, COLS, ROWS> const &m)
    {
        for (int i = 0; i < COLS; ++i)
            m_data[i] = (vec_t<T,ROWS>)m[i];
    }

    inline vec_t<T,ROWS>& operator[](size_t n) { return m_data[n]; }
    inline vec_t<T,ROWS> const& operator[](size_t n) const { return m_data[n]; }

    template<class U>
    friend std::ostream &operator<<(std::ostream &stream,
                                    mat_t<U,COLS,ROWS> const &m);

private:
    vec_t<T,ROWS> m_data[COLS];
};

/*
 * 2×2-element matrices
 */

template <typename T>
struct LOL_ATTR_NODISCARD mat_t<T, 2, 2>
  : public linear_ops::base<vec_t<T,2>>
{
    static int const count = 2;
    typedef T scalar_element;
    typedef vec_t<T,2> element;
    typedef mat_t<T,2,2> type;

    inline mat_t() {}
    inline mat_t(vec_t<T,2> v0, vec_t<T,2> v1)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ v0, v1 } {}
#else
      : m_v0(v0), m_v1(v1) {}
#endif

    explicit inline mat_t(T const &val)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ vec_t<T,2>(val, T(0)),
                vec_t<T,2>(T(0), val) } {}
#else
      : m_v0(val, T(0)),
        m_v1(T(0), val) {}
#endif

    explicit inline mat_t(mat_t<T,4,4> const &m)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ m[0].xy, m[1].xy } {}
#else
      : m_v0(m[0].xy), m_v1(m[1].xy) {}
#endif

    /* Explicit constructor for type conversion */
    template<typename U>
    explicit inline mat_t(mat_t<U,2,2> const &m)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ (element)m[0], (element)m[1] } {}
#else
      : m_v0((element)m[0]), m_v1((element)m[1]) {}
#endif

#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
    inline vec_t<T,2>& operator[](size_t n) { return m_data[n]; }
    inline vec_t<T,2> const& operator[](size_t n) const { return m_data[n]; }
#else
    inline vec_t<T,2>& operator[](size_t n) { return (&m_v0)[n]; }
    inline vec_t<T,2> const& operator[](size_t n) const { return (&m_v0)[n]; }
#endif

    /* Helpers for transformation matrices */
    static mat_t<T,2,2> rotate(T radians);
    static inline mat_t<T,2,2> rotate(mat_t<T,2,2> m, T radians)
    {
        return rotate(radians) * m;
    }

    void printf() const;
    std::string tostring() const;

    static const mat_t<T,2,2> identity;

private:
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
    vec_t<T,2> m_data[2];
#else
    vec_t<T,2> m_v0, m_v1;
#endif
};

static_assert(sizeof(imat2) == 16, "sizeof(imat2) == 16");
static_assert(sizeof(f16mat2) == 8, "sizeof(f16mat2) == 8");
static_assert(sizeof(mat2) == 16, "sizeof(mat2) == 16");
static_assert(sizeof(dmat2) == 32, "sizeof(dmat2) == 32");

/*
 * 3×3-element matrices
 */

template <typename T>
struct LOL_ATTR_NODISCARD mat_t<T, 3, 3>
  : public linear_ops::base<vec_t<T,3>>
{
    static int const count = 3;
    typedef T scalar_element;
    typedef vec_t<T,3> element;
    typedef mat_t<T,3,3> type;

    inline mat_t() {}
    inline mat_t(vec_t<T,3> v0, vec_t<T,3> v1, vec_t<T,3> v2)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ v0, v1, v2 } {}
#else
      : m_v0(v0), m_v1(v1), m_v2(v2) {}
#endif

    explicit inline mat_t(T const &val)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ vec_t<T,3>(val, (T)0, (T)0),
                vec_t<T,3>((T)0, val, (T)0),
                vec_t<T,3>((T)0, (T)0, val) } {}
#else
      : m_v0(val, (T)0, (T)0),
        m_v1((T)0, val, (T)0),
        m_v2((T)0, (T)0, val) {}
#endif

    explicit inline mat_t(mat_t<T,2,2> m, T const &val = T(1))
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ vec_t<T,3>(m[0], (T)0),
                vec_t<T,3>(m[1], (T)0),
                vec_t<T,3>((T)0, (T)0, val) } {}
#else
      : m_v0(m[0], (T)0),
        m_v1(m[1], (T)0),
        m_v2((T)0, (T)0, val) {}
#endif

    explicit inline mat_t(mat_t<T,4,4> const &m)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ m[0].xyz, m[1].xyz, m[2].xyz } {}
#else
      : m_v0(m[0].xyz), m_v1(m[1].xyz), m_v2(m[2].xyz) {}
#endif

    /* Explicit constructor for type conversion */
    template<typename U>
    explicit inline mat_t(mat_t<U,3,3> const &m)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ (element)m[0], (element)m[1], (element)m[2] } {}
#else
      : m_v0((element)m[0]), m_v1((element)m[1]), m_v2((element)m[2]) {}
#endif

    explicit mat_t(quat_t<T> const &q);

#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
    inline vec_t<T,3>& operator[](size_t n) { return m_data[n]; }
    inline vec_t<T,3> const& operator[](size_t n) const { return m_data[n]; }
#else
    inline vec_t<T,3>& operator[](size_t n) { return (&m_v0)[n]; }
    inline vec_t<T,3> const& operator[](size_t n) const { return (&m_v0)[n]; }
#endif

    /* Helpers for transformation matrices */
    static mat_t<T,3,3> scale(T x);
    static mat_t<T,3,3> scale(T x, T y, T z);
    static mat_t<T,3,3> scale(vec_t<T,3> v);
    static mat_t<T,3,3> rotate(T radians, T x, T y, T z);
    static mat_t<T,3,3> rotate(T radians, vec_t<T,3> v);

    static mat_t<T,3,3> fromeuler_xyz(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_xzy(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_yxz(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_yzx(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_zxy(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_zyx(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_xyz(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_xzy(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_yxz(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_yzx(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_zxy(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_zyx(T phi, T theta, T psi);

    static mat_t<T,3,3> fromeuler_xyx(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_xzx(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_yxy(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_yzy(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_zxz(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_zyz(vec_t<T,3> const &v);
    static mat_t<T,3,3> fromeuler_xyx(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_xzx(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_yxy(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_yzy(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_zxz(T phi, T theta, T psi);
    static mat_t<T,3,3> fromeuler_zyz(T phi, T theta, T psi);

    static inline mat_t<T,3,3> rotate(mat_t<T,3,3> m, T radians, vec_t<T,3> v)
    {
        return rotate(radians, v) * m;
    }

    void printf() const;
    std::string tostring() const;

    static const mat_t<T,3,3> identity;

private:
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
    vec_t<T,3> m_data[3];
#else
    vec_t<T,3> m_v0, m_v1, m_v2;
#endif
};

static_assert(sizeof(imat3) == 36, "sizeof(imat3) == 36");
static_assert(sizeof(f16mat3) == 18, "sizeof(f16mat3) == 18");
static_assert(sizeof(mat3) == 36, "sizeof(mat3) == 36");
static_assert(sizeof(dmat3) == 72, "sizeof(dmat3) == 72");

/*
 * 4×4-element matrices
 */

template <typename T>
struct LOL_ATTR_NODISCARD mat_t<T, 4, 4>
  : public linear_ops::base<vec_t<T,4>>
{
    static int const count = 4;
    typedef T scalar_element;
    typedef vec_t<T,4> element;
    typedef mat_t<T,4,4> type;

    inline mat_t() {}
    inline mat_t(vec_t<T,4> v0, vec_t<T,4> v1, vec_t<T,4> v2, vec_t<T,4> v3)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ v0, v1, v2, v3 } {}
#else
      : m_v0(v0), m_v1(v1), m_v2(v2), m_v3(v3) {}
#endif

    explicit inline mat_t(T const &val)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ vec_t<T,4>(val, (T)0, (T)0, (T)0),
                vec_t<T,4>((T)0, val, (T)0, (T)0),
                vec_t<T,4>((T)0, (T)0, val, (T)0),
                vec_t<T,4>((T)0, (T)0, (T)0, val) } {}
#else
      : m_v0(val, (T)0, (T)0, (T)0),
        m_v1((T)0, val, (T)0, (T)0),
        m_v2((T)0, (T)0, val, (T)0),
        m_v3((T)0, (T)0, (T)0, val) {}
#endif

    explicit inline mat_t(mat_t<T,2,2> m, T const &val = T(1))
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ vec_t<T,4>(m[0], (T)0, (T)0),
                vec_t<T,4>(m[1], (T)0, (T)0),
                vec_t<T,4>((T)0, (T)0, val, (T)0),
                vec_t<T,4>((T)0, (T)0, (T)0, val) } {}
#else
      : m_v0(m[0], (T)0, (T)0),
        m_v1(m[1], (T)0, (T)0),
        m_v2((T)0, (T)0, val, (T)0),
        m_v3((T)0, (T)0, (T)0, val) {}
#endif

    explicit inline mat_t(mat_t<T,3,3> m, T const &val = T(1))
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ vec_t<T,4>(m[0], (T)0),
                vec_t<T,4>(m[1], (T)0),
                vec_t<T,4>(m[2], (T)0),
                vec_t<T,4>((T)0, (T)0, (T)0, val) } {}
#else
      : m_v0(m[0], (T)0),
        m_v1(m[1], (T)0),
        m_v2(m[2], (T)0),
        m_v3((T)0, (T)0, (T)0, val) {}
#endif

    /* Explicit constructor for type conversion */
    template<typename U>
    explicit inline mat_t(mat_t<U,4,4> const &m)
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
      : m_data{ (element)m[0], (element)m[1],
                (element)m[2], (element)m[3] } {}
#else
      : m_v0((element)m[0]), m_v1((element)m[1]),
        m_v2((element)m[2]), m_v3((element)m[3]) {}
#endif

    explicit mat_t(quat_t<T> const &q);

#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
    inline vec_t<T,4>& operator[](size_t n) { return m_data[n]; }
    inline vec_t<T,4> const& operator[](size_t n) const { return m_data[n]; }
#else
    inline vec_t<T,4>& operator[](size_t n) { return (&m_v0)[n]; }
    inline vec_t<T,4> const& operator[](size_t n) const { return (&m_v0)[n]; }
#endif

    /* Helpers for transformation matrices */
    static mat_t<T,4,4> translate(T x, T y, T z);
    static mat_t<T,4,4> translate(vec_t<T,3> v);

    static inline mat_t<T,4,4> scale(T x)
    {
        return mat_t<T,4,4>(mat_t<T,3,3>::scale(x), (T)1);
    }

    static inline mat_t<T,4,4> scale(T x, T y, T z)
    {
        return mat_t<T,4,4>(mat_t<T,3,3>::scale(x, y, z), (T)1);
    }

    static inline mat_t<T,4,4> scale(vec_t<T,3> v)
    {
        return mat_t<T,4,4>(mat_t<T,3,3>::scale(v), (T)1);
    }

    static inline mat_t<T,4,4> translate(mat_t<T,4,4> const &m, vec_t<T,3> v)
    {
        return translate(v) * m;
    }

    static inline mat_t<T,4,4> rotate(T radians, T x, T y, T z)
    {
        return mat_t<T,4,4>(mat_t<T,3,3>::rotate(radians, x, y, z), (T)1);
    }

    static inline mat_t<T,4,4> rotate(T radians, vec_t<T,3> v)
    {
        return mat_t<T,4,4>(mat_t<T,3,3>::rotate(radians, v), (T)1);
    }

    static inline mat_t<T,4,4> rotate(mat_t<T,4,4> &m, T radians, vec_t<T,3> v)
    {
        return rotate(radians, v) * m;
    }

    static mat_t<T,4,4> fromeuler_xyz(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_xzy(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_yxz(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_yzx(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_zxy(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_zyx(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_xyz(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_xzy(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_yxz(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_yzx(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_zxy(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_zyx(T phi, T theta, T psi);

    static mat_t<T,4,4> fromeuler_xyx(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_xzx(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_yxy(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_yzy(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_zxz(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_zyz(vec_t<T,3> const &v);
    static mat_t<T,4,4> fromeuler_xyx(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_xzx(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_yxy(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_yzy(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_zxz(T phi, T theta, T psi);
    static mat_t<T,4,4> fromeuler_zyz(T phi, T theta, T psi);

    /* Helpers for view matrices */
    static mat_t<T,4,4> lookat(vec_t<T,3> eye, vec_t<T,3> center, vec_t<T,3> up);

    /* Helpers for projection matrices; FOV values are in radians */
    static mat_t<T,4,4> ortho(T left, T right, T bottom, T top, T near, T far);
    static mat_t<T,4,4> ortho(T width, T height, T near, T far);
    static mat_t<T,4,4> frustum(T left, T right, T bottom, T top, T near, T far);
    static mat_t<T,4,4> perspective(T fov_y, T width, T height, T near, T far);
    static mat_t<T,4,4> shifted_perspective(T fov_y, T screen_size, T screen_ratio_yx, T near, T far);

    void printf() const;
    std::string tostring() const;

    static const mat_t<T,4,4> identity;

private:
#if LOL_FEATURE_CXX11_ARRAY_INITIALIZERS
    vec_t<T,4> m_data[4];
#else
    vec_t<T,4> m_v0, m_v1, m_v2, m_v3;
#endif
};

static_assert(sizeof(imat4) == 64, "sizeof(imat4) == 64");
static_assert(sizeof(f16mat4) == 32, "sizeof(f16mat4) == 32");
static_assert(sizeof(mat4) == 64, "sizeof(mat4) == 64");
static_assert(sizeof(dmat4) == 128, "sizeof(dmat4) == 128");

/*
 * stdstream method implementations
 */

template<class U, int COLS, int ROWS>
std::ostream &operator<<(std::ostream &stream,
                         mat_t<U,COLS,ROWS> const &m)
{
    for (int y = 0; y < ROWS; ++y)
    {
        stream << (y == 0 ? "(" : ", ");
        for (int x = 0; x < COLS; ++x)
            stream << (x == 0 ? "(" : ", ") << m[x][y];
        stream << ")";
    }
    return stream << ")";
}

/*
 * Transpose any matrix
 */

template<typename T, int COLS, int ROWS>
static inline mat_t<T, ROWS, COLS> transpose(mat_t<T, COLS, ROWS> const &m)
{
    mat_t<T, ROWS, COLS> ret;
    for (int i = 0; i < COLS; ++i)
        for (int j = 0; j < ROWS; ++j)
            ret[j][i] = m[i][j];
    return ret;
}

/*
 * Compute a square submatrix, useful for minors and cofactor matrices
 */

template<typename T, int N>
mat_t<T, N - 1, N - 1> submatrix(mat_t<T, N, N> const &m, int i, int j)
{
    ASSERT(i >= 0); ASSERT(j >= 0); ASSERT(i < N); ASSERT(j < N);

    mat_t<T, N - 1, N - 1> ret;
    for (int i2 = 0; i2 < N - 1; ++i2)
        for (int j2 = 0; j2 < N - 1; ++j2)
            ret[i2][j2] = m[i2 + (i2 >= i)][j2 + (j2 >= j)];

    return ret;
}

/*
 * Compute square matrix cofactor
 */

template<typename T, int N> LOL_ATTR_NODISCARD
T cofactor(mat_t<T, N, N> const &m, int i, int j)
{
    ASSERT(i >= 0); ASSERT(j >= 0); ASSERT(i < N); ASSERT(j < N);
    T tmp = determinant(submatrix(m, i, j));
    return ((i + j) & 1) ? -tmp : tmp;
}

template<typename T> LOL_ATTR_NODISCARD
T cofactor(mat_t<T, 2, 2> const &m, int i, int j)
{
    /* This specialisation shouldn't be needed, but Visual Studio. */
    ASSERT(i >= 0); ASSERT(j >= 0); ASSERT(i < 2); ASSERT(j < 2);
    T tmp = m[1 - i][1 - j];
    return (i ^ j) ? -tmp : tmp;
}

/*
 * Compute LU-decomposition
 */

template<typename T, int N>
void lu_decomposition(mat_t<T, N, N> const &m, mat_t<T, N, N> & L, mat_t<T, N, N> & U)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            T sum = 0;
            for (int k = 0 ; k < min(i, j) ; ++k)
                sum += L[k][j] * U[i][k];

            if (i < j)
            {
                U[i][j] = 0;
                L[i][j] = (m[i][j] - sum) / U[i][i];
            }
            else /* if (i >= j) */
            {
                L[i][j] = i == j ? T(1) : T(0);
                U[i][j] = (m[i][j] - sum) / L[j][j];
            }
        }
    }
}

/*
 * Compute square matrix determinant, with a specialisation for 1×1 matrices
 */

template<typename T, int N> LOL_ATTR_NODISCARD
T determinant(mat_t<T, N, N> const &m)
{
    mat_t<T, N, N> L, U;
    vec_t<int, N> P = p_vector(m);
    lu_decomposition(permute_rows(m, P), L, U);

    T det = 1;
    for (int i = 0 ; i < N ; ++i)
        det *= U[i][i];

    return permutation_det(P) * det;
}

template<typename T> LOL_ATTR_NODISCARD
T const & determinant(mat_t<T, 1, 1> const &m)
{
    return m[0][0];
}

/*
 * Compute permutation vector of a square matrix
 */

template<typename T, int N>
vec_t<int, N> p_vector(mat_t<T, N, N> const & m)
{
    vec_t<int, N> used;
    vec_t<int, N> permutation;

    for (int i = 0 ; i < N ; ++i)
    {
        used[i] = 0;
        permutation[i] = -1;
    }

    for (int i = 0 ; i < N ; ++i)
    {
        int index_max = -1;

        for (int j = 0 ; j < N ; ++j)
        {
            if (!used[j] && (index_max == -1 || abs(m[i][j]) > abs(m[i][index_max])))
                index_max = j;
        }

        ASSERT(index_max != -1);
        ASSERT(index_max < N);

        permutation[i] = index_max;
        used[index_max] = 1;
    }

    return permutation;
}

/*
 * Compute the permutation of a square matrix according to the permutation vector
 */

template<typename T, int N>
mat_t<T, N, N> permute_rows(mat_t<T, N, N> const & m, vec_t<int, N> const & permutation)
{
    mat_t<T, N, N> result;

    for (int i = 0 ; i < N ; ++i)
    {
        for (int j = 0 ; j < N ; ++j)
        {
            result[i][j] = m[i][permutation[j]];
        }
    }

    return result;
}

template<typename T, int N>
mat_t<T, N, N> permute_cols(mat_t<T, N, N> const & m, vec_t<int, N> const & permutation)
{
    mat_t<T, N, N> result;

    for (int i = 0 ; i < N ; ++i)
    {
        for (int j = 0 ; j < N ; ++j)
        {
            result[i][j] = m[permutation[i]][j];
        }
    }

    return result;
}

template<int N>
vec_t<int, N> p_transpose(vec_t<int, N> P)
{
    vec_t<int, N> result;

    for (int i = 0 ; i < N ; ++i)
        result[P[i]] = i;

    return result;
}

/*
 * Compute the determinant of a permutation square matrix corresponding to the permutation vector
 */

template<int N> LOL_ATTR_NODISCARD
int permutation_det(vec_t<int, N> const & permutation)
{
    int result = 1;

    for (int i = 0 ; i < N ; ++i)
    {
        int relative_index = permutation[i];

        for (int j = 0 ; j < i ; ++j)
            if (permutation[j] < permutation[i])
                --relative_index;

        result *= (relative_index % 2 ? -1 : 1);
    }

    return result;
}

/*
 * Compute L matrix inverse
 */

template<typename T, int N>
mat_t<T, N, N> l_inverse(mat_t<T, N, N> const & L)
{
    mat_t<T, N, N> ret;

    for (int i = 0 ; i < N ; ++i)
    {
        for (int j = i ; j >= 0 ; --j)
        {
            T sum = 0;

            for (int k = i ; k > j ; --k)
                sum += ret[k][i] * L[j][k];

            ret[j][i] = ((i == j ? 1 : 0) - sum) / L[j][j];
        }
    }

    return ret;
}

/*
 * Compute U matrix inverse
 */

template<typename T, int N>
mat_t<T, N, N> u_inverse(mat_t<T, N, N> const & U)
{
    mat_t<T, N, N> ret;

    for (int i = 0 ; i < N ; ++i)
    {
        for (int j = i ; j < N ; ++j)
        {
            T sum = 0;

            for (int k = i ; k < j ; ++k)
                sum += ret[k][i] * U[j][k];

            ret[j][i] = ((i == j ? 1 : 0) - sum) / U[j][j];
        }
    }

    return ret;
}

/*
 * Compute square matrix inverse
 */

template<typename T, int N>
mat_t<T, N, N> inverse(mat_t<T, N, N> const &m)
{
    mat_t<T, N, N> L, U;
    vec_t<int, N> P = p_vector(m);

    lu_decomposition(permute_rows(m, P), L, U);

    mat_t<T, N, N> ret = permute_cols(u_inverse(U) * l_inverse(L), p_transpose(P));

    return ret;
}

/*
 * Matrix-vector and vector-matrix multiplication
 */

template<typename T, int COLS, int ROWS, int SWIZZLE>
static inline vec_t<T, ROWS> operator *(mat_t<T, COLS, ROWS> const &m,
                                        vec_t<T, COLS, SWIZZLE> const &v)
{
    vec_t<T, ROWS> ret(T(0));
    for (int i = 0; i < COLS; ++i)
        ret += m[i] * v[i];
    return ret;
}

template<typename T, int COLS, int ROWS, int SWIZZLE>
static inline vec_t<T, COLS> operator *(vec_t<T, ROWS, SWIZZLE> const &v,
                                        mat_t<T, COLS, ROWS> const &m)
{
    vec_t<T, COLS> ret(T(0));
    for (int i = 0; i < COLS; ++i)
        ret[i] = dot(v, m[i]);
    return ret;
}

/*
 * Matrix-matrix multiplication
 */

template<typename T, int COLS, int N, int ROWS>
static inline mat_t<T, COLS, ROWS> operator *(mat_t<T, N, ROWS> const &a,
                                              mat_t<T, COLS, N> const &b)
{
    mat_t<T, COLS, ROWS> ret;
    for (int i = 0; i < COLS; ++i)
        ret[i] = a * b[i];
    return ret;
}

template<typename T, int N>
static inline mat_t<T, N, N> &operator *=(mat_t<T, N, N> &a,
                                          mat_t<T, N, N> const &b)
{
    return a = a * b;
}

/*
 * Vector-vector outer product
 */

template<typename T, int COLS, int ROWS>
static inline mat_t<T, COLS, ROWS> outer(vec_t<T, ROWS> const &a,
                                         vec_t<T, COLS> const &b)
{
    /* Valid cast because mat_t and vec_t have similar layouts */
    return *reinterpret_cast<mat_t<T, 1, ROWS> const *>(&a)
         * *reinterpret_cast<mat_t<T, COLS, 1> const *>(&b);
}

/*
 * Matrix-matrix outer product (Kronecker product)
 */

template<typename T, int COLS1, int COLS2, int ROWS1, int ROWS2>
static inline mat_t<T, COLS1 * COLS2, ROWS1 * ROWS2>
outer(mat_t<T, COLS1, ROWS1> const &a, mat_t<T, COLS2, ROWS2> const &b)
{
    mat_t<T, COLS1 * COLS2, ROWS1 * ROWS2> ret;
    for (int i1 = 0; i1 < COLS1; ++i1)
        for (int i2 = 0; i2 < COLS2; ++i2)
        {
            /* Valid cast because mat_t and vec_t have similar layouts */
            *reinterpret_cast<mat_t<T, ROWS1, ROWS2> *>(&ret[i1 * COLS2 + i2])
                = outer(b[i2], a[i1]);
        }
    return ret;
}

/*
 * Constants
 */

template<typename T>
mat_t<T,2,2> const mat_t<T,2,2>::identity = mat_t<T,2,2>((T)1);
template<typename T>
mat_t<T,3,3> const mat_t<T,3,3>::identity = mat_t<T,3,3>((T)1);
template<typename T>
mat_t<T,4,4> const mat_t<T,4,4>::identity = mat_t<T,4,4>((T)1);

} /* namespace lol */

#if _WIN32
#   pragma pop_macro("near")
#   pragma pop_macro("far")
#endif

