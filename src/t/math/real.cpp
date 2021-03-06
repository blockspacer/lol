//
//  Lol Engine — Unit tests
//
//  Copyright © 2010—2019 Sam Hocevar <sam@hocevar.net>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#include <lol/engine-internal.h>

#include <cmath>

#include <lolunit.h>

namespace lol
{

lolunit_declare_fixture(real_test)
{
    lolunit_declare_test(float_to_real)
    {
        float a1 = real(0.0f);
        float a2 = real(-0.0f);
        float a3 = real(1.0f);
        float a4 = real(-1.0f);
        float a5 = real(1.5f);
        float a6 = real(12345678.0f);

        lolunit_assert_equal(a1, 0.0f);
        lolunit_assert_equal(a2, -0.0f);
        lolunit_assert_equal(a3, 1.0f);
        lolunit_assert_equal(a4, -1.0f);
        lolunit_assert_equal(a5, 1.5f);
        lolunit_assert_equal(a6, 12345678.0f);
    }

    lolunit_declare_test(double_to_real)
    {
        double a1 = real(0.0);
        double a2 = real(-0.0);
        double a3 = real(1.0);
        double a4 = real(-1.0);
        double a5 = real(1.5);
        double a6 = real(1234567876543210.0);

        lolunit_assert_doubles_equal(a1, 0.0, 0.0);
        lolunit_assert_doubles_equal(a2, -0.0, 0.0);
        lolunit_assert_doubles_equal(a3, 1.0, 0.0);
        lolunit_assert_doubles_equal(a4, -1.0, 0.0);
        lolunit_assert_doubles_equal(a5, 1.5, 0.0);
        lolunit_assert_doubles_equal(a6, 1234567876543210.0, 0.0);
    }

    lolunit_declare_test(long_double_to_real)
    {
        real r((long double)1 / 3);

        // Count how many bits are correct
        int ok_bits = 0;
        while (int(r) == 0)
        {
            r = (r * 4) - 1;
            ok_bits += 2;
        }

        // 64 bits means our precision is better than double
        lolunit_assert_greater(ok_bits, 64);
    }

    lolunit_declare_test(uint32_to_real)
    {
        uint32_t a = 0;
        uint32_t b = 1;
        uint32_t c = UINT32_MAX;
        uint32_t d = UINT32_MAX ^ (UINT32_MAX >> 1);
        uint32_t e = UINT32_MAX ^ (UINT32_MAX >> 1) ^ 1;

        lolunit_assert_equal(a, (uint32_t)(real)a);
        lolunit_assert_equal(b, (uint32_t)(real)b);
        lolunit_assert_equal(c, (uint32_t)(real)c);
        lolunit_assert_equal(d, (uint32_t)(real)d);
        lolunit_assert_equal(e, (uint32_t)(real)e);
    }

    lolunit_declare_test(int32_to_real)
    {
        int32_t a = 0;
        int32_t b = 1;
        int32_t c = -1;
        int32_t d = INT32_MIN;
        int32_t e = INT32_MAX;

        lolunit_assert_equal(a, (int32_t)(real)a);
        lolunit_assert_equal(b, (int32_t)(real)b);
        lolunit_assert_equal(c, (int32_t)(real)c);
        lolunit_assert_equal(d, (int32_t)(real)d);
        lolunit_assert_equal(e, (int32_t)(real)e);
    }

    lolunit_declare_test(uint64_to_real)
    {
        uint64_t a = 0;
        uint64_t b = 1;
        uint64_t c = UINT64_MAX;
        uint64_t d = UINT64_MAX ^ (UINT64_MAX >> 1);
        uint64_t e = UINT64_MAX ^ (UINT64_MAX >> 1) ^ 1;

        lolunit_assert_equal(a, (uint64_t)(real)a);
        lolunit_assert_equal(b, (uint64_t)(real)b);
        lolunit_assert_equal(c, (uint64_t)(real)c);
        lolunit_assert_equal(d, (uint64_t)(real)d);
        lolunit_assert_equal(e, (uint64_t)(real)e);
    }

    lolunit_declare_test(int64_to_real)
    {
        int64_t a = 0;
        int64_t b = 1;
        int64_t c = -1;
        int64_t d = INT64_MIN;
        int64_t e = INT64_MAX;

        lolunit_assert_equal(a, (int64_t)(real)a);
        lolunit_assert_equal(b, (int64_t)(real)b);
        lolunit_assert_equal(c, (int64_t)(real)c);
        lolunit_assert_equal(d, (int64_t)(real)d);
        lolunit_assert_equal(e, (int64_t)(real)e);
    }

    lolunit_declare_test(constants)
    {
        double a0 = real::R_0();
        double a1 = real::R_1();
        double a2 = real::R_2();
        double a10 = real::R_10();

        lolunit_assert_equal(a0, 0.0);
        lolunit_assert_equal(a1, 1.0);
        lolunit_assert_equal(a2, 2.0);
        lolunit_assert_equal(a10, 10.0);

        double b1 = log(real::R_E());
        double b2 = log2(real::R_2());
        lolunit_assert_equal(b1, 1.0);
        lolunit_assert_equal(b2, 1.0);

        double c1 = exp(inverse(real::R_LOG2E()));
        double c2 = log(exp2(real::R_LOG2E()));
        lolunit_assert_equal(c1, 2.0);
        lolunit_assert_equal(c2, 1.0);

        double d1 = exp(inverse(real::R_LOG10E()));
        lolunit_assert_equal(d1, 10.0);

        double e1 = exp(real::R_LN2());
        lolunit_assert_equal(e1, 2.0);

        double f1 = exp(real::R_LN10());
        lolunit_assert_equal(f1, 10.0);

        double g1 = sin(real::R_PI());
        double g2 = cos(real::R_PI());
        lolunit_assert_doubles_equal(g1, 0.0, 1e-100);
        lolunit_assert_equal(g2, -1.0);

        double h1 = sin(real::R_PI_2());
        double h2 = cos(real::R_PI_2());
        lolunit_assert_equal(h1, 1.0);
        lolunit_assert_doubles_equal(h2, 0.0, 1e-100);

        double i1 = sin(real::R_PI_4()) * sin(real::R_PI_4());
        double i2 = cos(real::R_PI_4()) * cos(real::R_PI_4());
        lolunit_assert_equal(i1, 0.5);
        lolunit_assert_equal(i2, 0.5);
    }

    lolunit_declare_test(init)
    {
        real r;
        float f1 = (float)r;

        lolunit_assert_equal(f1, 0.0f);

        rcmplx q;
        float f2 = (float)q.x;
        float f3 = (float)q.y;

        lolunit_assert_equal(f2, 0.0f);
        lolunit_assert_equal(f3, 0.0f);
    }

    lolunit_declare_test(string_to_real)
    {
        float a1 = real("0");
        float a2 = real("1");
        float a3 = real("-1");
        /* 2^-128 * 2^128 */
        float a4 = real("0.0000000000000000000000000000000000000029387358770"
                        "557187699218413430556141945466638919302188037718792"
                        "6569604314863681793212890625")
                 * real("340282366920938463463374607431768211456");

        lolunit_assert_equal(a1, 0.0f);
        lolunit_assert_equal(a2, 1.0f);
        lolunit_assert_equal(a3, -1.0f);
        lolunit_assert_equal(a4, 1.0f);
    }

    lolunit_declare_test(unary_minus)
    {
        float a1 = - real(1.0f);
        float a2 = - real(-1.0f);
        float a3 = - real(0.0f);
        float a4 = - real(-0.0f);

        lolunit_assert_equal(a1, -1.0f);
        lolunit_assert_equal(a2, 1.0f);
        lolunit_assert_equal(a3, -0.0f);
        lolunit_assert_equal(a4, 0.0f);
    }

    lolunit_declare_test(comparison)
    {
        lolunit_assert(real(1.0f) > real(0.5f));
        lolunit_assert(real(1.0f) >= real(0.5f));
        lolunit_assert(real(1.0f) >= real(1.0f));

        lolunit_assert(real(-1.0f) < real(-0.5f));
        lolunit_assert(real(-1.0f) <= real(-0.5f));
        lolunit_assert(real(-1.0f) <= real(-1.0f));

        lolunit_assert(real(-1.0f) < real(0.5f));
        lolunit_assert(real(-0.5f) < real(1.0f));
        lolunit_assert(real(-1.0f) <= real(0.5f));
        lolunit_assert(real(-0.5f) <= real(1.0f));

        lolunit_assert(real(1.0f) > real(-0.5f));
        lolunit_assert(real(0.5f) > real(-1.0f));
        lolunit_assert(real(1.0f) >= real(-0.5f));
        lolunit_assert(real(0.5f) >= real(-1.0f));
    }

    lolunit_declare_test(addition)
    {
        float a1 = real(1.0f) + real(0.0f);
        float a2 = real(0.0f) + real(1.0f);
        float a3 = real(1.0f) + real(1.0f);
        float a4 = real(-1.0f) + real(-1.0f);
        float a5 = real(1.0f) + real(0.125f);
        double a6 = real(3.13609818956293918)
                  + real(0.00005972154828114);
        float a7 = real(1.0f) + real(-0.125f);
        double a8 = real(0.10000000002) + real(-2.0e-11);

        lolunit_assert_equal(a1, 1.0f);
        lolunit_assert_equal(a2, 1.0f);
        lolunit_assert_equal(a3, 2.0f);
        lolunit_assert_equal(a4, -2.0f);
        lolunit_assert_equal(a5, 1.125f);
        lolunit_assert_doubles_equal(a6, 3.1361579, 0.000001);
        lolunit_assert_equal(a7, 0.875f);
        lolunit_assert_doubles_equal(a8, 0.1, 1.0e-13);
    }

    lolunit_declare_test(subtraction)
    {
        float a1 = real(1.0f) + real(1e20f) - real(1e20f);

        lolunit_assert_equal(a1, 1.0f);
    }

    lolunit_declare_test(multiplication)
    {
        real x(1.25f);
        real y(1.5f);
        real z(1.99999f);
        real w(-1.5f);

        float m1 = x * x;
        float m2 = y * y;
        float m3 = z * z;
        float m4 = w * w;

        lolunit_assert_equal(m1, 1.25f * 1.25f);
        lolunit_assert_equal(m2, 1.5f * 1.5f);
        lolunit_assert_equal(m3, 1.99999f * 1.99999f);
        lolunit_assert_equal(m4, -1.5f * -1.5f);
    }

    lolunit_declare_test(exact_division)
    {
        float m1 = real::R_1() / real::R_1();
        float m2 = real::R_2() / real::R_1();
        float m3 = real::R_1() / real::R_2();
        float m4 = real::R_2() / real::R_2();
        float m5 = real::R_1() / -real::R_2();

        lolunit_assert_equal(m1, 1.0f);
        lolunit_assert_equal(m2, 2.0f);
        lolunit_assert_equal(m3, 0.5f);
        lolunit_assert_equal(m4, 1.0f);
        lolunit_assert_equal(m5, -0.5f);
    }

    lolunit_declare_test(inexact_division)
    {
        /* 1 / 3 * 3 should be close to 1... check that it does not differ
         * by more than 2^-k where k is the number of bits in the mantissa. */
        real a = real::R_1() / real::R_3() * real::R_3();
        real b = ldexp(real::R_1() - a, a.total_bits());

        lolunit_assert_lequal((double)fabs(b), 1.0);
    }

    lolunit_declare_test(real_sqrt)
    {
        double sqrt0 = sqrt(real(0));
        double sqrt1 = sqrt(real(1));
        double sqrt2 = sqrt(real(2));
        double sqrt3 = sqrt(real(3));
        double sqrt4 = sqrt(real(4));
        double sqrt5 = sqrt(real(5));
        double sqrt6 = sqrt(real(6));
        double sqrt7 = sqrt(real(7));
        double sqrt8 = sqrt(real(8));

        lolunit_assert_doubles_equal(sqrt0, sqrt(0.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt1, sqrt(1.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt2, sqrt(2.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt3, sqrt(3.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt4, sqrt(4.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt5, sqrt(5.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt6, sqrt(6.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt7, sqrt(7.0), 1e-8);
        lolunit_assert_doubles_equal(sqrt8, sqrt(8.0), 1e-8);
    }

    lolunit_declare_test(real_cbrt)
    {
        double cbrtm8 = cbrt(real(-8));
        double cbrtm7 = cbrt(real(-7));
        double cbrtm6 = cbrt(real(-6));
        double cbrtm5 = cbrt(real(-5));
        double cbrtm4 = cbrt(real(-4));
        double cbrtm3 = cbrt(real(-3));
        double cbrtm2 = cbrt(real(-2));
        double cbrtm1 = cbrt(real(-1));
        double cbrt0 = cbrt(real(0));
        double cbrt1 = cbrt(real(1));
        double cbrt2 = cbrt(real(2));
        double cbrt3 = cbrt(real(3));
        double cbrt4 = cbrt(real(4));
        double cbrt5 = cbrt(real(5));
        double cbrt6 = cbrt(real(6));
        double cbrt7 = cbrt(real(7));
        double cbrt8 = cbrt(real(8));

        lolunit_assert_doubles_equal(cbrtm8, cbrt(-8.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm7, cbrt(-7.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm6, cbrt(-6.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm5, cbrt(-5.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm4, cbrt(-4.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm3, cbrt(-3.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm2, cbrt(-2.0), 1e-8);
        lolunit_assert_doubles_equal(cbrtm1, cbrt(-1.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt0, cbrt(0.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt1, cbrt(1.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt2, cbrt(2.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt3, cbrt(3.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt4, cbrt(4.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt5, cbrt(5.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt6, cbrt(6.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt7, cbrt(7.0), 1e-8);
        lolunit_assert_doubles_equal(cbrt8, cbrt(8.0), 1e-8);
    }

    lolunit_declare_test(real_ldexp)
    {
        real a1(1.5);
        real a2(-1.5);
        real a3(0.0);

        lolunit_assert_equal((double)ldexp(a1, 7), 192.0);
        lolunit_assert_equal((double)ldexp(a1, -7), 0.01171875);

        lolunit_assert_equal((double)ldexp(a2, 7), -192.0);
        lolunit_assert_equal((double)ldexp(a2, -7), -0.01171875);

        lolunit_assert_equal((double)ldexp(a3, 7), 0.0);
        lolunit_assert_equal((double)ldexp(a3, -7), 0.0);
    }

    lolunit_declare_test(real_bool)
    {
        real a = 0.0;
        lolunit_assert(!a);

        a = -0.0;
        lolunit_assert(!a);

        a = 1234.0;
        lolunit_assert(a);
        lolunit_assert(!!a);

        a = -1234.0;
        lolunit_assert(a);
        lolunit_assert(!!a);
    }

    lolunit_declare_test(real_asin_acos)
    {
        double tests[] =
        {
            -1024.0, -1023.0, -513.0, -512.0, -511.0, -1.0, -0.0,
            0.0, 1.0, 511.0, 512.0, 513.0, 1023.0, 1024.0
        };

        for (double test : tests)
        {
            double a = test / 1024;
            double b = sin(asin((real)a));
            double c = cos(acos((real)a));

            lolunit_set_context(a);
            lolunit_assert_doubles_equal(b, a, 1e-100);
            lolunit_assert_doubles_equal(c, a, 1e-100);
        }
    }

    lolunit_declare_test(floor_ceil_etc)
    {
        double tests[] =
        {
            -2.0,  -2.0, -2.0,  -2.0,
            -1.5,  -2.0, -1.0,  -2.0,
            -1.0,  -1.0, -1.0,  -1.0,
            -0.0,  -0.0, -0.0,  -0.0,
             0.0,   0.0,  0.0,   0.0,
             0.25,  0.0,  1.0,   0.0,
             0.375, 0.0,  1.0,   0.0,
             0.5,   0.0,  1.0,   1.0,
             1.0,   1.0,  1.0,   1.0,
             1.5,   1.0,  2.0,   2.0,
             2.0,   2.0,  2.0,   2.0,
             2.5,   2.0,  3.0,   3.0,
             3.0,   3.0,  3.0,   3.0,
            8192.0,     8192.0, 8192.0, 8192.0,
            8192.03125, 8192.0, 8193.0, 8192.0,
            8192.5,     8192.0, 8193.0, 8193.0,
            8193.0,     8193.0, 8193.0, 8193.0,
            549755813888.0,     549755813888.0, 549755813888.0, 549755813888.0,
            549755813888.03125, 549755813888.0, 549755813889.0, 549755813888.0,
            549755813888.5,     549755813888.0, 549755813889.0, 549755813889.0,
            549755813889.0,     549755813889.0, 549755813889.0, 549755813889.0,
        };

        for (unsigned int n = 0; n < sizeof(tests) / sizeof(*tests); n += 4)
        {
            double a0 = floor((real)tests[n]);
            double b0 = tests[n + 1];
            double a1 = ceil((real)tests[n]);
            double b1 = tests[n + 2];
            double a2 = round((real)tests[n]);
            double b2 = tests[n + 3];

            lolunit_assert_equal(b0, a0);
            lolunit_assert_equal(b1, a1);
            lolunit_assert_equal(b2, a2);
        }
    }

    lolunit_declare_test(real_pow)
    {
        double a1 = pow(-real::R_2(), real::R_2());
        double b1 = 4.0;
        lolunit_assert_doubles_equal(a1, b1, 1.0e-13);

        double a2 = pow(-real::R_2(), real::R_3());
        double b2 = -8.0;
        lolunit_assert_doubles_equal(a2, b2, 1.0e-13);
    }
};

} /* namespace lol */

