#include "common.h"
#include <cstdio>
#include <cstdlib>

GLuint CreateAndCompileShader(GLenum type, const char* source) {
    GLint success;
    GLchar msg[512];

    GLuint handle = glCreateShader(type);
    if (!handle) {
        fprintf(stderr, "%u: cannot create shader", type);
        exit(EXIT_FAILURE);
        return 0;
    }
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
        fprintf(stderr, "%u: %s\n", type, msg);
        exit(EXIT_FAILURE);
    }

    return handle;
}

GLuint LinkShaders(const std::vector<GLuint>& shaders) {
    const GLuint program{glCreateProgram()};
    for (const auto shader : shaders) glAttachShader(program, shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char buf[512];
        glGetProgramInfoLog(program, sizeof(buf), nullptr, buf);
        fprintf(stderr, "Link error: %s", buf);
        exit(EXIT_FAILURE);
    }
    for (const auto shader : shaders) glDeleteShader(shader);

    return program;
}

GLuint CreateProgram(const char* vertex, const char* fragment) {
    const GLuint vs{CreateAndCompileShader(GL_VERTEX_SHADER, vertex)};
    const GLuint fs{CreateAndCompileShader(GL_FRAGMENT_SHADER, fragment)};
    return LinkShaders({vs, fs});
}

GLuint CreateProgram(const char* vertex, const char* geometry,
                     const char* fragment) {
    const GLuint vs{CreateAndCompileShader(GL_VERTEX_SHADER, vertex)};
    const GLuint gs{CreateAndCompileShader(GL_GEOMETRY_SHADER, geometry)};
    const GLuint fs{CreateAndCompileShader(GL_FRAGMENT_SHADER, fragment)};
    return LinkShaders({vs, gs, fs});
}

GLuint GenBuffer() {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    return buffer;
}

GLuint CreateBufferAndBind(GLenum target, std::size_t size, const void* data,
                           GLenum usage) {
    const GLuint buffer = GenBuffer();
    glBindBuffer(target, buffer);
    glBufferData(target, size, data, usage);
    return buffer;
}

GLuint GenVertexArray() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return vao;
}

GLuint CreateVertexArrayAndBind() {
    GLuint vao = GenVertexArray();
    glBindVertexArray(vao);
    return vao;
}
