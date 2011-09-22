//
// Lol Engine
//
// Copyright: (c) 2010-2011 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <cstring>
#include <cstdio>

#include "core.h"

using namespace std;

namespace lol
{

real::real(float f)
{
    union { float f; uint32_t x; } u = { f };

    uint32_t sign = u.x & 0x80000000u;
    uint32_t exponent = (u.x >> 23) & 0xff;

    switch (exponent)
    {
    case 0x00:
    case 0xff:
        m_signexp = sign | exponent;
        break;
    default:
        m_signexp = sign | (exponent + (1 << 30) - (1 << 7));
        break;
    }

    m_mantissa[0] = u.x >> 7;
    m_mantissa[1] = u.x << 9;
    memset(m_mantissa + 2, 0, sizeof(m_mantissa) - sizeof(m_mantissa[0]));
}

real::operator float() const
{
    union { float f; uint32_t x; } u;

    uint32_t sign = m_signexp & 0x80000000u;
    uint32_t exponent = m_signexp & 0x7fffffffu;
    uint32_t mantissa = (m_mantissa[0] << 7) | (m_mantissa[1] >> 9);

    int e = (int)exponent - (1 << 30) + (1 << 7);

    if (e < 0)
        u.x = sign;
    else if (e >= 0xff)
        u.x = sign | (0xff << 23);
    else
        u.x = sign | (e << 23) | mantissa;

    return u.f;
}

real real::operator -() const
{
    real ret = *this;
    ret.m_signexp ^= 0x80000000u;
    return ret;
}

real real::operator +(real const &x) const
{
    if (x.m_signexp << 1 == 0)
        return *this;

    /* Ensure both arguments are positive. Otherwise, switch signs,
     * or replace + with -). */
    if (m_signexp >> 31)
        return -(-*this + -x);

    if (x.m_signexp >> 31)
        return *this - x;

    /* Ensure *this is the larger exponent (no need to be strictly larger,
     * as in subtraction). Otherwise, switch. */
    if ((m_signexp << 1) < (x.m_signexp << 1))
        return x + *this;

    real ret;

    int e1 = m_signexp - (1 << 30) + 1;
    int e2 = x.m_signexp - (1 << 30) + 1;

    int bigoff = (e1 - e2) / (sizeof(uint16_t) * 8);
    int off = e1 - e2 - bigoff * (sizeof(uint16_t) * 8);

    ret.m_signexp = m_signexp;

    uint32_t carry = 0;
    for (int i = BIGITS; i--; )
    {
        carry += m_mantissa[i];
        if (i - bigoff >= 0)
            carry += x.m_mantissa[i - bigoff] >> off;
        else if (i - bigoff == -1)
            carry += 0x0001u >> off;

        if (i - bigoff > 0)
            carry += (x.m_mantissa[i - bigoff - 1] << (16 - off)) & 0xffffu;
        else if (i - bigoff == 0)
            carry += 0x0001u << (16 - off);

        ret.m_mantissa[i] = carry;
        carry >>= 16;
    }

    /* Renormalise in case we overflowed the mantissa */
    if (carry)
    {
        carry--;
        for (int i = 0; i < BIGITS; i++)
        {
            uint16_t tmp = ret.m_mantissa[i];
            ret.m_mantissa[i] = (carry << 15) | (tmp >> 1);
            carry = tmp & 0x0001u;
        }
        ret.m_signexp++;
    }

    return ret;
}

real real::operator -(real const &x) const
{
    if (x.m_signexp << 1 == 0)
        return *this;

    /* Ensure both arguments are positive. Otherwise, switch signs,
     * or replace - with +). */
    if (m_signexp >> 31)
        return -(-*this + x);

    if (x.m_signexp >> 31)
        return (*this) + (-x);

    /* Ensure *this is larger than x */
    if (*this < x)
        return -(x - *this);

    real ret;

    int e1 = m_signexp - (1 << 30) + 1;
    int e2 = x.m_signexp - (1 << 30) + 1;

    int bigoff = (e1 - e2) / (sizeof(uint16_t) * 8);
    int off = e1 - e2 - bigoff * (sizeof(uint16_t) * 8);

    ret.m_signexp = m_signexp;

    int32_t carry = 0;
    for (int i = 0; i < bigoff; i++)
    {
        carry -= x.m_mantissa[BIGITS - i];
        carry = (carry & 0xffff0000u) | (carry >> 16);
    }
    carry -= x.m_mantissa[BIGITS - 1 - bigoff] & ((1 << off) - 1);
    carry /= (1 << off);

    for (int i = BIGITS; i--; )
    {
        carry += m_mantissa[i];
        if (i - bigoff >= 0)
            carry -= x.m_mantissa[i - bigoff] >> off;
        else if (i - bigoff == -1)
            carry -= 0x0001u >> off;

        if (i - bigoff > 0)
            carry -= (x.m_mantissa[i - bigoff - 1] << (16 - off)) & 0xffffu;
        else if (i - bigoff == 0)
            carry -= 0x0001u << (16 - off);

        ret.m_mantissa[i] = carry;
        carry = (carry & 0xffff0000u) | (carry >> 16);
    }

    carry += 1;

    /* Renormalise if we underflowed the mantissa */
    if (carry == 0)
    {
        /* How much do we need to shift the mantissa? FIXME: this could
         * be computed above */
        off = 0;
        for (int i = 0; i < BIGITS; i++)
        {
            if (!ret.m_mantissa[i])
            {
                off += sizeof(uint16_t) * 8;
                continue;
            }

            for (uint16_t tmp = ret.m_mantissa[i]; tmp < 0x8000u; tmp <<= 1)
                off++;
            break;
        }
        if (off == BIGITS * sizeof(uint16_t) * 8)
            ret.m_signexp &= 0x80000000u;
        else
        {
            off++; /* Shift one more to get rid of the leading one */
            ret.m_signexp -= off;

            bigoff = off / (sizeof(uint16_t) * 8);
            off -= bigoff * sizeof(uint16_t) * 8;

            for (int i = 0; i < BIGITS; i++)
            {
                uint16_t tmp = 0;
                if (i + bigoff < BIGITS)
                    tmp |= ret.m_mantissa[i + bigoff] << off;
                if (i + bigoff + 1 < BIGITS)
                    tmp |= ret.m_mantissa[i + bigoff + 1] >> (16 - off);
                ret.m_mantissa[i] = tmp;
            }
        }
    }

    return ret;
}

real real::operator *(real const &x) const
{
    real ret;

    ret.m_signexp = (m_signexp ^ x.m_signexp) & 0x80000000u;
    int e = (m_signexp & 0x7fffffffu) - (1 << 30) + 1
          + (x.m_signexp & 0x7fffffffu) - (1 << 30) + 1;

    /* Accumulate low order product; no need to store it, we just
     * want the carry value */
    uint32_t carry = 0;
    for (int i = 0; i < BIGITS; i++)
    {
        for (int j = 0; j < i + 1; j++)
            carry += m_mantissa[BIGITS - 1 - j]
                   * x.m_mantissa[BIGITS - 1 + j - i];
        carry >>= 16;
    }

    for (int i = 0; i < BIGITS; i++)
    {
        for (int j = i + 1; j < BIGITS; j++)
            carry += m_mantissa[BIGITS - 1 - j]
                   * x.m_mantissa[j - 1 - i];

        carry += m_mantissa[BIGITS - 1 - i];
        carry += x.m_mantissa[BIGITS - 1 - i];
        ret.m_mantissa[BIGITS - 1 - i] = carry & 0xffffu;
        carry >>= 16;
    }

    /* Renormalise in case we overflowed the mantissa */
    if (carry)
    {
        carry--;
        for (int i = 0; i < BIGITS; i++)
        {
            uint16_t tmp = ret.m_mantissa[i];
            ret.m_mantissa[i] = (carry << 15) | (tmp >> 1);
            carry = tmp & 0x0001u;
        }
        e++;
    }

    ret.m_signexp |= e + (1 << 30) - 1;

    return ret;
}

real real::operator /(real const &x) const
{
    return *this * fres(x);
}

bool real::operator <(real const &x) const
{
    /* Ensure both numbers are positive */
    if (m_signexp >> 31)
        return (x.m_signexp >> 31) ? -*this > -x : true;

    if (x.m_signexp >> 31)
        return false;

    /* Compare all relevant bits */
    if (m_signexp != x.m_signexp)
        return m_signexp < x.m_signexp;

    for (int i = 0; i < BIGITS; i++)
        if (m_mantissa[i] != x.m_mantissa[i])
            return m_mantissa[i] < x.m_mantissa[i];

    return false;
}

bool real::operator <=(real const &x) const
{
    return !(*this > x);
}

bool real::operator >(real const &x) const
{
    /* Ensure both numbers are positive */
    if (m_signexp >> 31)
        return (x.m_signexp >> 31) ? -*this < -x : false;

    if (x.m_signexp >> 31)
        return true;

    /* Compare all relevant bits */
    if (m_signexp != x.m_signexp)
        return m_signexp > x.m_signexp;

    for (int i = 0; i < BIGITS; i++)
        if (m_mantissa[i] != x.m_mantissa[i])
            return m_mantissa[i] > x.m_mantissa[i];

    return false;
}

bool real::operator >=(real const &x) const
{
    return !(*this < x);
}

real fres(real const &x)
{
    if (!(x.m_signexp << 1))
    {
        real ret = x;
        ret.m_signexp = x.m_signexp | 0x7fffffffu;
        ret.m_mantissa[0] = 0;
        return ret;
    }

    /* Use the system's float inversion to approximate 1/x */
    union { float f; uint32_t x; } u = { 1.0f }, v = { 1.0f };
    v.x |= (uint32_t)x.m_mantissa[0] << 7;
    v.x |= (uint32_t)x.m_mantissa[1] >> 9;
    v.f = 1.0 / v.f;

    real ret;
    ret.m_mantissa[0] = (v.x >> 7) & 0xffffu;
    ret.m_mantissa[1] = (v.x << 9) & 0xffffu;
    /* Better convergence with the mantissa zeroed. */
    memset(ret.m_mantissa + 2, 0, (real::BIGITS - 2) * sizeof(uint16_t));

    uint32_t sign = x.m_signexp & 0x80000000u;
    ret.m_signexp = sign;

    int exponent = (x.m_signexp & 0x7fffffffu) + 1;
    exponent = -exponent + (v.x >> 23) - (u.x >> 23);
    ret.m_signexp |= (exponent - 1) & 0x7fffffffu;

    /* Five steps of Newton-Raphson seems enough for 32-bigit reals. */
    real two(2.0f);
    ret = ret * (two - ret * x);
    ret = ret * (two - ret * x);
    ret = ret * (two - ret * x);
    ret = ret * (two - ret * x);
    ret = ret * (two - ret * x);

    return ret;
}

void real::print() const
{
    printf("%x  %08x  ", m_signexp >> 31, (m_signexp << 1) >> 1);
    for (int i = 0; i < BIGITS; i++)
        printf("%04x ", m_mantissa[i]);
    printf("\n");
}

} /* namespace lol */

