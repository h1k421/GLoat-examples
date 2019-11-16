#pragma once

#include <gloat.hpp>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GLSLC_LANGUAGE_GLSL,
    GLSLC_LANGUAGE_GLSL_ES,
    GLSLC_LANGUAGE_SPIRV
} glslc_language_t;

typedef enum {

} glslc_debug_output_t;

typedef enum {

} glslc_optimization_t;

typedef struct {
    uint32_t api_major;
    uint32_t api_minor;
    uint32_t gpu_major;
    uint32_t gpu_minor;
    uint32_t unk_version;
    char unk3[0x20];
} glslc_version_t;

typedef struct {
    bool separate_object : 1;
    bool output_assembly : 1;
    // TODO: this seems to be for separing stages in independant binaries, need to check that.
    bool output_gpu_something : 1;
    bool output_unk0 : 1;
    bool output_shader_reflection : 1;
    glslc_language_t language : 4;
    glslc_debug_output_t debug_output_level : 4;
    uint32_t unk0 : 4;
    // TODO: this seems to force GLSLC to only output a ready gpu shader.
    bool output_only_x1: 1;
    bool unk1 : 1;
    bool unk2 : 1;
    uint32_t unk3 : 6;
    glslc_optimization_t optimization_level : 3;

    uint32_t unk4 : 3;
    bool unk5 : 1;

} glslc_options_flag_t;

typedef struct {
    char **paths;
    uint32_t paths_count;
    char padding[0x20];
} glslc_option_include_t;

typedef struct {
    void *unk;
    glslc_options_flag_t flags;
    glslc_option_include_t include;
    char unk1[0x30];

    // maybe padding? reserved?
    char unk2[0x20];

} glslc_options_t;

typedef enum {
    GLSLC_SHADER_VERTEX,
    GLSLC_SHADER_FRAGMENT,
    GLSLC_SHADER_GEOMETRY,
    // TODO find others?
} glslc_shader_type_t;

typedef struct {
    const char **sources;
    glslc_shader_type_t *sources_types;
    uint8_t sources_count;
    char unk[7];

    const char **spirv_entrypoints;
    const uint32_t *spirv_modules_sizes;
    // FIXME: probably related to spirv stuffs
    const void *unk1;

} glslc_env_input_shaders_t;

typedef enum {
    GLSLC_INIT_STATUS_UNINITIALIZED,
    GLSLC_INIT_STATUS_INITIALIZED,
    GLSLC_INIT_STATUS_ALLOC_FAIL,
    GLSLC_INIT_STATUS_NO_ALLOC_CALLBACK,
} glslc_init_status_t;

typedef struct {
    char *compilation_logs;
    uint32_t compilation_logs_length;
    uint8_t success;
    uint8_t out_of_memory_error;
    char unk[0x1e];

} glslc_compilation_status_t;

typedef enum {
    GLSLC_COMPILATION_OUTPUT_GPU = 0,
    // TODO: others???
} glslc_compilation_output_section_type_t;

typedef struct {
    uint32_t size;
    uint32_t offset;
    glslc_compilation_output_section_type_t type;
    char unk[0x20];
} glslc_compilation_output_section_header_t;

typedef struct {
    glslc_compilation_output_section_header_t common_header;
    glslc_shader_type_t shader_type;
    uint32_t control_offset;
    uint32_t data_offset;
    uint32_t data_size;
    uint32_t control_size;
    uint32_t unk0;
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t unk4;
    uint32_t unk5;
    char unk[0x38];
} glslc_compilation_output_section_gpu_header_t;

typedef union
{
    glslc_compilation_output_section_header_t common_header;
    glslc_compilation_output_section_gpu_header_t gpu_header;
} glslc_compilation_output_section_union_header_t;


_Static_assert(sizeof(glslc_compilation_output_section_gpu_header_t) == 0x90, "glslc_compilation_output_section_gpu_header_t isn't valid");

typedef struct {
    uint32_t magic;
    uint32_t padding; // I guess?
    glslc_options_flag_t options_flags;
    glslc_version_t version;
    uint32_t size;
    uint32_t offset;
    uint32_t sections_count;
    char reserved[0x40];
    glslc_compilation_output_section_union_header_t sections_headers[1];
} glslc_compilation_output_t;

//_Static_assert(sizeof(glslc_compilation_output_t) == 0x1B0, "glslc_compilation_output_t isn't valid");


typedef struct {
    glslc_compilation_status_t *status;
    glslc_compilation_output_t *output;
    char unk[0x20];
} glslc_compilation_info_t;

typedef struct {
    glslc_compilation_info_t *compilation_info;
    void *unk1;
    void *unk2; // TODO: allocated with a calloc(1, 0x98)
    glslc_options_t options;
    glslc_env_input_shaders_t input_shaders;

    glslc_init_status_t status;
    // TODO: maybe padding? reserved?
    char unk3[0x1c];
} glslc_env_t;

_Static_assert(sizeof(glslc_options_flag_t) == 0x8, "glslc_options_flag_t isn't valid");
_Static_assert(sizeof(glslc_option_include_t) == 0x30, "glslc_option_include_t isn't valid");
_Static_assert(sizeof(glslc_options_t) == 0x90, "glslc_options_t isn't valid");
_Static_assert(sizeof(glslc_compilation_status_t) == 0x30, "glslc_compilation_status_t isn't valid");
_Static_assert(sizeof(glslc_compilation_info_t) == 0x30, "glslc_compilation_info_t isn't valid");
_Static_assert(sizeof(glslc_compilation_output_section_header_t) == 0x2c, "glslc_compilation_output_section_header_t isn't valid");
_Static_assert(sizeof(glslc_version_t) == 0x34, "glslc_version_t isn't valid");
_Static_assert(sizeof(glslc_env_t) == 0xF8, "glslc_env_t isn't valid");

WEAK void glslcSetAllocator(void* allocator(size_t, size_t, void*),
                            void deallocator(void*, void*),
                            void* rallocator(void*, size_t, void*),
                            void* unk);

WEAK glslc_options_t glslcGetDefaultOptions();
WEAK glslc_version_t glslcGetVersion();

WEAK uint32_t glslcInitialize(glslc_env_t *env);
WEAK uint32_t glslcCompile(glslc_env_t *env);

WEAK void glslcFinalize(glslc_env_t *env);

#ifdef __cplusplus
}
#endif