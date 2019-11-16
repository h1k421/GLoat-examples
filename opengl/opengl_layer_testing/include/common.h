#pragma once

#include <cstdio>
#include <vector>

#include <gloat.hpp>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GL/glcorearb.h"

#define OPENGL_CORE(major, minor) \
    bool g_opengl_core = true;    \
    int g_opengl_major = major;   \
    int g_opengl_minor = minor;

#define OPENGL_COMPAT(major, minor) \
    bool g_opengl_core = false;     \
    int g_opengl_major = major;     \
    int g_opengl_minor = minor;

GLuint CreateAndCompileShader(GLenum type, const char* source);

GLuint LinkShaders(const std::vector<GLuint>& shaders);

GLuint CreateProgram(const char* vertex, const char* fragment);

GLuint CreateProgram(const char* vertex, const char* geometry,
                     const char* fragment);

GLuint GenBuffer();

GLuint CreateBufferAndBind(GLenum target, std::size_t size, const void* data,
                           GLenum usage = GL_STATIC_DRAW);

GLuint GenVertexArray();

GLuint CreateVertexArrayAndBind();
