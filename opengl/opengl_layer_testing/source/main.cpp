#include <cassert>
#include <cstdlib>
#include "common.h"

OPENGL_CORE(4, 3)

static const char* VertexShader = R"text(
    #version 430 core
    layout (location = 0) in vec2 vsPos;
    layout (location = 1) in vec2 vsTexcoord;
    layout (location = 0) out vec2 fsTexcoord;
    void main() {
        gl_Position = vec4(vsPos, 0, 1);
        fsTexcoord = vsTexcoord;
    }
)text";

const char* FragShader = R"text(
    #version 430 core
    layout (location = 0) in vec2 fsTexcoord;
    layout (location = 0) out vec4 color;
    uniform sampler2DArray tex;
    layout (location = 0) uniform float index;
    void main() {
        color = texture(tex, vec3(fsTexcoord, index));
    }
)text";

static GLuint s_program_misaka;
static GLuint s_vao, s_vbo;
static GLuint s_tex;

GLuint loadDDS(const char* path);

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                      GLsizei length, const GLchar* message,
                      const void* userParam) {
    printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
           severity, message);
}

void init() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);

    s_program_misaka = CreateProgram(VertexShader, FragShader);

    struct Vertex {
        float position[2];
        float texcoord[2];
    };

    /*static const Vertex vertices[] = {
        {{-1.0f, -1.0f}, {0.0f, 0.0f}},
        {{1.0f, -1.0f}, {1.0f, 0.0f}},
        {{-1.0f, 1.0f}, {0.0f, 1.0f}},
        {{1.0f, 1.0f}, {1.0f, 1.0f}},
    };*/

    static const Vertex vertices[] = {
        {{-1.0f, 1.0f}, {0.0f, 0.0f}},
        {{1.0f, 1.0f}, {1.0f, 0.0f}},
        {{-1.0f, -1.0f}, {0.0f, 1.0f}},
        {{1.0f, -1.0f}, {1.0f, 1.0f}},
    };

    s_vao = CreateVertexArrayAndBind();

    s_vbo = CreateBufferAndBind(GL_ARRAY_BUFFER, sizeof(vertices), vertices);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texcoord));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

// Not on Switch?
#ifndef __SWITCH__
#define TEXTURE_NAME "opengl_layer_testing.dds"
#else
#define TEXTURE_NAME "sdmc:/opengl_layer_testing.dds"
#endif

    s_tex = loadDDS(TEXTURE_NAME);
    if (s_tex == 0) {
        std::abort();
    }
}

int g_Layer = 0;

void chooseLayer(int layerId) {
    g_Layer = layerId;
}

void render() {
    glUseProgram(s_program_misaka);
    glBindVertexArray(s_vao);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1f(0, static_cast<GLfloat>(g_Layer));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void quit() {}