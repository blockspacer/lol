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

#include "core.h"
#include "lolgl.h"

#if defined _WIN32 && defined USE_D3D9
#   define FAR
#   define NEAR
#   include <d3d9.h>
#endif

using namespace std;

#if defined USE_D3D9
extern IDirect3DDevice9 *g_d3ddevice;
#elif defined _XBOX
extern D3DDevice *g_d3ddevice;
#endif

namespace lol
{

//
// The FrameBufferData class
// -------------------------
//

class FrameBufferData
{
    friend class FrameBuffer;

    ivec2 m_size;

#if defined USE_D3D9
#elif defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    GLuint m_fbo, m_texture, m_depth;
#endif
};

//
// The FrameBuffer class
// ----------------------
//

FrameBuffer::FrameBuffer(ivec2 size)
  : m_data(new FrameBufferData)
{
    m_data->m_size = size;
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    GLenum format = GL_RGBA8;
    GLenum depth = GL_DEPTH_COMPONENT;
    GLenum wrapmode = GL_REPEAT;
    GLenum filtering = GL_NEAREST;

    glGenFramebuffers(1, &m_data->m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_data->m_fbo);

    glGenTextures(1, &m_data->m_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_data->m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)wrapmode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)wrapmode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLenum)filtering);
    glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)format, size.x, size.y, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_data->m_texture, 0);
    m_data->m_depth = GL_INVALID_INDEX;
    if (depth != GL_INVALID_INDEX)
    {
        glGenRenderbuffers(1, &m_data->m_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_data->m_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, depth, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, m_data->m_depth);
    }
    glCheckFramebufferStatus(GL_FRAMEBUFFER);

    Unbind();
#endif
}

FrameBuffer::~FrameBuffer()
{
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    glDeleteFramebuffers(1, &m_data->m_fbo);
    glDeleteTextures(1, &m_data->m_texture);
    if (m_data->m_depth != GL_INVALID_INDEX)
        glDeleteRenderbuffers(1, &m_data->m_depth);
#endif
    delete m_data;
}

int FrameBuffer::GetTexture() const
{
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    return m_data->m_texture;
#endif
}

void FrameBuffer::Clear(vec4 color)
{
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void FrameBuffer::Clear(vec4 color, float depth)
{
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    glClearColor(color.r, color.g, color.b, color.a);
    //glClearDepthf(depth);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

void FrameBuffer::Bind()
{
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    glBindFramebuffer(GL_FRAMEBUFFER, m_data->m_fbo);
#endif
}

void FrameBuffer::Unbind()
{
#if defined USE_D3D9 || defined _XBOX
#elif !defined __CELLOS_LV2__ && !defined __ANDROID__
    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
#endif
}

} /* namespace lol */
