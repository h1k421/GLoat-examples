#include <cassert>
#include <cstdlib>

#include "glslc.h"
#include "utils.hpp"

glslc_env_t glslc_env;

nn::Result fs_write(char const *path, void *data, size_t data_size);

static const char VertexShaderSourceLayer[] = R"text(
    #version 430 core
    layout (location = 0) in vec2 vsPos;
    void main() {
        gl_Position = vec4(vsPos, 0, 1);
    }
)text";

static const char FragmentShaderSourceLayer[] = R"text(
    #version 430 core
    //layout (location = 0) uniform vec4 color;
    layout (location = 0) out vec4 fbColor;
    void main() {
        fbColor = vec4(1, 1, 1, 1);
    }
)text";

static const char GeometryShaderSourceLayer[] = R"text(
#version 430 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
)text";

static const char *sources[] = {
    FragmentShaderSourceLayer,
    VertexShaderSourceLayer,
    GeometryShaderSourceLayer
};

static glslc_shader_type_t sources_types[] = {
    GLSLC_SHADER_FRAGMENT,
    GLSLC_SHADER_VERTEX,
    GLSLC_SHADER_GEOMETRY
};

uint32_t get_shader_length(uint32_t *gpu_code, uint32_t max_size) {
    if (max_size <= 0x80) {
        return max_size;
    }

    uint32_t real_size = 0x80;
    uint64_t opcode = 0;
    while ((opcode >> 52 & 0xfff8) != 0x50b0) {
        uint32_t index = real_size / 4;
        uint32_t word0 = gpu_code[index];
        uint32_t word1 = gpu_code[index + 1];

        opcode = word0 | (uint64_t)word1 << 32;

        if (opcode == 0) {
            break;
        }

        real_size += 8;
    }

    return real_size - 0x80;
}

void dump_shader(char *base, glslc_compilation_output_section_gpu_header_t *gpu_section, const char *dump_base_path, bool is_code) {
    void *gpu_code = base + gpu_section->data_offset;
    void *gpu_control = base + gpu_section->control_offset;
    uint32_t code_size = get_shader_length((uint32_t*)gpu_code, gpu_section->data_size);

    // ignore first part
    gpu_code = (char*)gpu_code + 0x30;

    // ignore the header if not a full dump
    if (is_code) {
        gpu_code = (char*)gpu_code + 0x50;
    } else {
        code_size += 0x50;
    }

    char path[0x400];

    snprintf(path, 0x400, "%s.code", dump_base_path);

    nn::Result result = fs_write(path, gpu_code, code_size);
    if (!result.Ok())
    {
        debug_log("Result: 0x%08x", result.GetValue());
        gloat::utils::Abort(&result);
    }

    snprintf(path, 0x400, "%s.control", dump_base_path);
    result = fs_write(path, gpu_control, gpu_section->control_size);
    if (!result.Ok())
    {
        debug_log("Result: 0x%08x", result.GetValue());
        gloat::utils::Abort(&result);
    }
}

extern "C" void appMain() {
    glslcSetAllocator(nv_alloc, nv_free, nv_realloc, nullptr);
    uint32_t glslc_res = glslcInitialize(&glslc_env);

    if (!glslc_res)
    {
        debug_log("glslcInitialize: 0x%08x", glslc_env.status);
        return;
    }

    glslc_env.input_shaders.sources = sources;
    glslc_env.input_shaders.sources_types = sources_types;
    glslc_env.input_shaders.sources_count = 3;

    glslcCompile(&glslc_env);

    if (!glslc_env.compilation_info->status->success) {
        debug_log("Compilation failed: %s\n", glslc_env.compilation_info->status->compilation_logs);
        return;
    } else {
        debug_log("Compilation success\n");
        glslc_compilation_output_t *output = glslc_env.compilation_info->output;
        for (uint32_t i = 0; i < output->sections_count; i++) {
            glslc_compilation_output_section_union_header_t *header = &output->sections_headers[i];

            if (header->common_header.type == GLSLC_COMPILATION_OUTPUT_GPU) {
                glslc_compilation_output_section_gpu_header_t *gpu_section = &header->gpu_header;

                char *data = ((char*)output) + gpu_section->common_header.offset;

                if (header->gpu_header.shader_type == GLSLC_SHADER_VERTEX) {
                    dump_shader(data, gpu_section, "sdmc:/vertex.bin", true);
                }

                if (header->gpu_header.shader_type == GLSLC_SHADER_FRAGMENT) {
                    dump_shader(data, gpu_section, "sdmc:/fragment.bin", true);
                }
            }
        }

    }

    glslcFinalize(&glslc_env);
    debug_log("All done\n");
}