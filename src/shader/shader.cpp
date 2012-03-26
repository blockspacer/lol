//
// Lol Engine
//
// Copyright: (c) 2010-2012 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://sam.zoy.org/projects/COPYING.WTFPL for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <cmath>
#include <cstring>
#include <cstdio>

#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include "core.h"
#include "lolgl.h"

using namespace std;

namespace lol
{

/*
 * Shader implementation class
 */

class ShaderData
{
    friend class Shader;

private:
#if defined _XBOX

#elif !defined __CELLOS_LV2__
    GLuint prog_id, vert_id, frag_id;
#else
    CGprogram vert_id, frag_id;
#endif
    uint32_t vert_crc, frag_crc;

    /* Shader patcher */
    static int GetVersion();
    static void Patch(char *dst, char const *vert, char const *frag);

    /* Global shader cache */
    static Shader *shaders[];
    static int nshaders;
};

Shader *ShaderData::shaders[256];
int ShaderData::nshaders = 0;

/*
 * Public Shader class
 */

Shader *Shader::Create(char const *vert, char const *frag)
{
    uint32_t new_vert_crc = Hash::Crc32(vert);
    uint32_t new_frag_crc = Hash::Crc32(frag);

    for (int n = 0; n < ShaderData::nshaders; n++)
    {
        if (ShaderData::shaders[n]->data->vert_crc == new_vert_crc
             && ShaderData::shaders[n]->data->frag_crc == new_frag_crc)
            return ShaderData::shaders[n];
    }

    Shader *ret = new Shader(vert, frag);
    ShaderData::shaders[ShaderData::nshaders] = ret;
    ShaderData::nshaders++;
    return ret;
}

void Shader::Destroy(Shader *shader)
{
    /* XXX: do nothing! the shader should remain in cache */
    (void)shader;
}

Shader::Shader(char const *vert, char const *frag)
  : data(new ShaderData())
{
#if !defined __CELLOS_LV2__
    char buf[4096], errbuf[4096];
    char const *shader = buf;
    GLint status;
    GLsizei len;
#else
    /* Initialise the runtime shader compiler. FIXME: this needs only
     * to be done once. */
    cgRTCgcInit();
#endif

    /* Compile vertex shader */
    data->vert_crc = Hash::Crc32(vert);
#if !defined __CELLOS_LV2__
    ShaderData::Patch(buf, vert, NULL);
    data->vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(data->vert_id, 1, &shader, NULL);
    glCompileShader(data->vert_id);

    glGetShaderiv(data->vert_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderInfoLog(data->vert_id, sizeof(errbuf), &len, errbuf);
        Log::Error("failed to compile vertex shader: %s", errbuf);
        Log::Error("shader source:\n%s\n", buf);
    }
#else
    data->vert_id = cgCreateProgram(cgCreateContext(), CG_SOURCE, vert,
                                    cgGLGetLatestProfile(CG_GL_VERTEX),
                                    NULL, NULL);
    if (data->vert_id == NULL)
    {
        Log::Error("failed to compile vertex shader");
        Log::Error("shader source:\n%s\n", vert);
    }
#endif

    /* Compile fragment shader */
    data->frag_crc = Hash::Crc32(frag);
#if !defined __CELLOS_LV2__
    ShaderData::Patch(buf, NULL, frag);
    data->frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(data->frag_id, 1, &shader, NULL);
    glCompileShader(data->frag_id);

    glGetShaderiv(data->frag_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glGetShaderInfoLog(data->frag_id, sizeof(errbuf), &len, errbuf);
        Log::Error("failed to compile fragment shader: %s", errbuf);
        Log::Error("shader source:\n%s\n", buf);
    }
#else
    data->frag_id = cgCreateProgram(cgCreateContext(), CG_SOURCE, frag,
                                    cgGLGetLatestProfile(CG_GL_FRAGMENT),
                                    NULL, NULL);
    if (data->frag_id == NULL)
    {
        Log::Error("failed to compile fragment shader");
        Log::Error("shader source:\n%s\n", frag);
    }
#endif

#if !defined __CELLOS_LV2__
    /* Create program */
    data->prog_id = glCreateProgram();
    glAttachShader(data->prog_id, data->vert_id);
    glAttachShader(data->prog_id, data->frag_id);

    glLinkProgram(data->prog_id);
    glValidateProgram(data->prog_id);
#endif
}

int Shader::GetAttribLocation(char const *attr) const
{
#if !defined __CELLOS_LV2__
    return glGetAttribLocation(data->prog_id, attr);
#else
    /* FIXME: need to differentiate between vertex and fragment program */
    return 0;
#endif
}

int Shader::GetUniformLocation(char const *uni) const
{
#if !defined __CELLOS_LV2__
    return glGetUniformLocation(data->prog_id, uni);
#else
    /* FIXME: need to differentiate between vertex and fragment program,
     * and replace the ugly cast. */
    CGparameter tmp = cgGetNamedParameter(data->vert_id, uni);
    if (tmp == NULL)
        tmp = cgGetNamedParameter(data->frag_id, uni);
    return (int)(intptr_t)tmp;
#endif
}

void Shader::SetUniform(int uni, float f)
{
#if !defined __CELLOS_LV2__
    glUniform1f(uni, f);
#else
    cgGLSetParameter1f((CGparameter)(intptr_t)uni, f);
#endif
}

void Shader::SetUniform(int uni, vec2 const &v)
{
#if !defined __CELLOS_LV2__
    glUniform2f(uni, v.x, v.y);
#else
    cgGLSetParameter2f((CGparameter)(intptr_t)uni, v.x, v.y);
#endif
}

void Shader::SetUniform(int uni, vec3 const &v)
{
#if !defined __CELLOS_LV2__
    glUniform3f(uni, v.x, v.y, v.z);
#else
    cgGLSetParameter3f((CGparameter)(intptr_t)uni, v.x, v.y, v.z);
#endif
}

void Shader::SetUniform(int uni, vec4 const &v)
{
    /* FIXME: use the array versions of these functions */
#if !defined __CELLOS_LV2__
    glUniform4f(uni, v.x, v.y, v.z, v.w);
#else
    cgGLSetParameter4f((CGparameter)(intptr_t)uni, v.x, v.y, v.z, v.w);
#endif
}

void Shader::SetUniform(int uni, mat4 const &m)
{
#if !defined __CELLOS_LV2__
    glUniformMatrix4fv(uni, 1, GL_FALSE, &m[0][0]);
#else
    cgGLSetMatrixParameterfc((CGparameter)(intptr_t)uni, &m[0][0]);
#endif
}

void Shader::Bind() const
{
#if !defined __CELLOS_LV2__
    glUseProgram(data->prog_id);
#else
    cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
    cgGLBindProgram(data->vert_id);
    cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));
    cgGLBindProgram(data->frag_id);
#endif
}

Shader::~Shader()
{
#if !defined __CELLOS_LV2__
    glDetachShader(data->prog_id, data->vert_id);
    glDetachShader(data->prog_id, data->frag_id);
    glDeleteShader(data->vert_id);
    glDeleteShader(data->frag_id);
    glDeleteProgram(data->prog_id);
#else
    cgDestroyProgram(data->vert_id);
    cgDestroyProgram(data->frag_id);
#endif
    delete data;
}

/* Try to detect shader compiler features */
int ShaderData::GetVersion()
{
    static int version = 0;

#if !defined __CELLOS_LV2__
    if (!version)
    {
        char buf[4096];
        GLsizei len;

        int id = glCreateShader(GL_VERTEX_SHADER);

        /* Can we compile 1.30 shaders? */
        char const *test130 =
            "#version 130\n"
            "void main() { gl_Position = vec4(0.0, 0.0, 0.0, 0.0); }";
        glShaderSource(id, 1, &test130, NULL);
        glCompileShader(id);
        glGetShaderInfoLog(id, sizeof(buf), &len, buf);
        if (len <= 0)
            version = 130;

        /* If not, can we compile 1.20 shaders? */
        if (!version)
        {
            char const *test120 =
                "#version 120\n"
                "void main() { gl_Position = vec4(0.0, 0.0, 0.0, 0.0); }";
            glShaderSource(id, 1, &test120, NULL);
            glCompileShader(id);
            glGetShaderInfoLog(id, sizeof(buf), &len, buf);
            if (len <= 0)
                version = 120;
        }

        /* Otherwise, assume we can compile 1.10 shaders. */
        if (!version)
            version = 110;

        glDeleteShader(id);
    }
#endif

    return version;
}

/* Simple shader source patching for old GLSL versions.
 * If supported version is 1.30, do nothing.
 * If supported version is 1.20:
 *  - replace "#version 130" with "#version 120"
 */
void ShaderData::Patch(char *dst, char const *vert, char const *frag)
{
    int ver_driver = GetVersion();

    strcpy(dst, vert ? vert : frag);
    if (ver_driver >= 130)
        return;

    int ver_shader = 110;
    char *parser = strstr(dst, "#version");
    if (parser)
        ver_shader = atoi(parser + strlen("#version"));

    if (ver_shader > 120 && ver_driver <= 120)
    {
        char const *end = dst + strlen(dst) + 1;

        /* Find main() */
        parser = strstr(dst, "main");
        if (!parser) return;
        parser = strstr(parser, "(");
        if (!parser) return;
        parser = strstr(parser, ")");
        if (!parser) return;
        parser = strstr(parser, "{");
        if (!parser) return;
        char *main = parser + 1;

        /* Perform main() replaces */
        char const * const main_replaces[] =
        {
#if 0
            "in vec2 in_Vertex;", "vec2 in_Vertex = gl_Vertex.xy;",
            "in vec3 in_Vertex;", "vec3 in_Vertex = gl_Vertex.xyz;",
            "in vec4 in_Vertex;", "vec4 in_Vertex = gl_Vertex.xyzw;",

            "in vec2 in_Color;", "vec2 in_Color = gl_Color.xy;",
            "in vec3 in_Color;", "vec3 in_Color = gl_Color.xyz;",
            "in vec4 in_Color;", "vec4 in_Color = gl_Color.xyzw;",

            "in vec2 in_MultiTexCoord0;",
               "vec2 in_MultiTexCoord0 = gl_MultiTexCoord0.xy;",
            "in vec2 in_MultiTexCoord1;",
               "vec2 in_MultiTexCoord1 = gl_MultiTexCoord1.xy;",
            "in vec2 in_MultiTexCoord2;",
               "vec2 in_MultiTexCoord2 = gl_MultiTexCoord2.xy;",
            "in vec2 in_MultiTexCoord3;",
               "vec2 in_MultiTexCoord3 = gl_MultiTexCoord3.xy;",
            "in vec2 in_MultiTexCoord4;",
               "vec2 in_MultiTexCoord4 = gl_MultiTexCoord4.xy;",
            "in vec2 in_MultiTexCoord5;",
               "vec2 in_MultiTexCoord5 = gl_MultiTexCoord5.xy;",
            "in vec2 in_MultiTexCoord6;",
               "vec2 in_MultiTexCoord6 = gl_MultiTexCoord6.xy;",
            "in vec2 in_MultiTexCoord7;",
               "vec2 in_MultiTexCoord7 = gl_MultiTexCoord7.xy;",
#endif

            NULL
        };

        for (char const * const *rep = main_replaces; rep[0]; rep += 2)
        {
            char *match = strstr(dst, rep[0]);
            if (match && match < main)
            {
                size_t l0 = strlen(rep[0]);
                size_t l1 = strlen(rep[1]);
                memmove(main + l1, main, end - main);
                memcpy(main, rep[1], l1);
                memset(match, ' ', l0);
                main += l1;
                end += l1;
            }
        }

        /* Perform small replaces */
        char const * const fast_replaces[] =
        {
            "#version 130", "#version 120",
            "in vec2", vert ? "attribute vec2" : "varying vec2",
            "in vec3", vert ? "attribute vec3" : "varying vec3",
            "in vec4", vert ? "attribute vec4" : "varying vec4",
            "in mat4", vert ? "attribute mat4" : "varying mat4",
            "out vec2", "varying vec2",
            "out vec3", "varying vec3",
            "out vec4", "varying vec4",
            "out mat4", "varying mat4",
            NULL
        };

        for (char const * const *rep = fast_replaces; rep[0]; rep += 2)
        {
            char *match;
            while ((match = strstr(dst, rep[0])))
            {
                size_t l0 = strlen(rep[0]);
                size_t l1 = strlen(rep[1]);

                if (l1 > l0)
                    memmove(match + l1, match + l0, (end - match) - l0);
                memcpy(match, rep[1], l1);
                if (l1 < l0)
                    memset(match + l0, ' ', l1 - l0);
                end += l1 - l0;
            }
        }
    }
}

} /* namespace lol */

