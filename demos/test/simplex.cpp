//
//  Simplex Noise Test Program
//
//  Copyright (c) 2010-2014 Sam Hocevar <sam@hocevar.net>
//            (c) 2013-2014 Guillaume Bittoun <guillaume.bittoun@gmail.com>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <lol/engine.h>

using namespace lol;

/* Constants to tweak */
float const zoom = 0.03f;
int const octaves = 1;

int main(int argc, char **argv)
{
    UNUSED(argc, argv);

    /* Create an image */
    ivec2 const size(1280, 720);
    Image img(size);
    array2d<vec4> &data = img.Lock2D<PixelFormat::RGBA_F32>();

    /* Declare plenty of allocators */
    simplex_interpolator<2> s2;
    simplex_interpolator<3> s3;
    simplex_interpolator<4> s4;
    simplex_interpolator<5> s5;
    simplex_interpolator<6> s6;
    simplex_interpolator<7> s7;
    simplex_interpolator<8> s8;
    simplex_interpolator<9> s9;
    simplex_interpolator<10> s10;
    simplex_interpolator<11> s11;
    simplex_interpolator<12> s12;

    /* Fill image with simplex noise */
    for (int j = 0; j < size.y; ++j)
    for (int i = 0; i < size.x; ++i)
    {
        int cell = j / (size.y / 2) * 3 + i / (size.x / 3);

        float x = (float)i, y = (float)j;
        float sum = 0.f, coeff = 0.f;
        int const max_k = 1 << octaves;

        for (int k = 1; k < max_k; k *= 2)
        {
            float t = 0.f;

            switch (cell)
            {
            case 0:
                t = s2.Interp(zoom * k * vec2(x, y));
                break;
            case 1:
                t = s3.Interp(zoom * k * vec3(x, 1.f, y));
                break;
            case 2:
                t = s4.Interp(zoom * k * vec4(x, 1.f, y, 2.f));
                break;
            case 3:
                t = s6.Interp(zoom * k * vec6(x, 1.f, 2.f, y, 3.f, 4.f));
                break;
            case 4:
                t = s8.Interp(zoom * k * vec8(x, 1.f, 2.f, 3.f, y, 4.f,
                                              5.f, 6.f));
                break;
            case 5:
                t = s12.Interp(zoom * k * vec12(x, 1.f, 2.f, 3.f, 4.f, 5.f,
                                                y, 6.f, 7.f, 8.f, 9.f, 10.f));
                break;
            default:
                break;
            }

            sum += 1.f / k * t;
            coeff += 1.f / k;
        }

        float c = saturate(0.5f + 0.5f * sum / coeff);
        data[i][j] = vec4(c, c, c, 1.f);
        //data[i][j] = Color::HSVToRGB(vec4(c, 1.0f, 0.5f, 1.f));
    }

#if 0
    /* Mark simplex vertices */
    vec2 diagonal = normalize(vec2(1.f));
    vec2 dx = mat2::rotate(60.f) * diagonal;
    vec2 dy = mat2::rotate(-60.f) * diagonal;
    for (int j = -100; j < 100; ++j)
    for (int i = -100; i < 100; ++i)
    {
        auto putpixel = [&](ivec2 p, vec4 c = vec4(1.f, 0.f, 1.f, 1.f))
        {
            if (p.x >= 0 && p.x < size.x - 1 && p.y >= 0 && p.y < size.y - 1)
                data[p.x][p.y] = c;
        };

        ivec2 coord = ivec2(i / zoom * dx + j / zoom * dy);

        vec2 g = s2.GetGradient((i + 0.1f) * dx + (j + 0.1f) * dy);
        for (int t = 0; t < 40; ++t)
            putpixel(coord + (ivec2)(g * (t / 2.f)), vec4(0.f, 1.f, 0.f, 1.f));

        putpixel(coord);
        putpixel(coord + ivec2(1, 0));
        putpixel(coord + ivec2(0, 1));
        putpixel(coord + ivec2(1, 1));
    }
#endif

    /* Save image */
    img.Unlock2D(data);
    img.Save("simplex.png");
}
