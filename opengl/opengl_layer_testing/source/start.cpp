#include <gloat.hpp>
#include <string>
#include <vector>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "glad/glad.h"

extern bool g_opengl_core;
extern int g_opengl_major;
extern int g_opengl_minor;

void fs_free(void *ptr, size_t unk)  // unk = size?
{
    free(ptr);
}

void init_fs(void) {
    nn::fs::SetAllocator(malloc, fs_free);

    const char *mountPoint = "sdmc";

    nn::Result result = nn::fs::MountSdCard(mountPoint);
    if (!result.Ok()) {
        debug_log("Result: 0x%08x", result.GetValue());
        gloat::utils::Abort(&result);
    }

    nn::fs::SetEnabledAutoAbort(false);
}

nn::Result fs_write(char const *path, void *data, size_t data_size) {
    nn::Result result = nn::fs::CreateFile(path, data_size);
    bool need_resize = false;

    if (!result.Ok()) {
        if (result.GetValue() != 0x402) {
            return result;
        }
        need_resize = true;
    }

    nn::fs::FileHandle handle;
    result = nn::fs::OpenFile(&handle, path,
                              nn::fs::FileOpenMode::FileOpenMode_Writable);
    if (!result.Ok()) {
        return result;
    }

    if (need_resize) {
        result = nn::fs::SetFileSize(handle, data_size);
        if (!result.Ok()) {
            return result;
        }
    }

    nn::fs::WriteOption writeOption;
    writeOption.mode = 1;

    result = nn::fs::WriteFile(handle, 0, data, data_size, writeOption);
    if (!result.Ok()) {
        return result;
    }

    result = nn::fs::FlushFile(handle);
    if (!result.Ok()) {
        return result;
    }

    nn::fs::CloseFile(handle);

    return result;
}

nn::Result fs_read(char const *path, void **data, long *data_size) {
    nn::fs::FileHandle handle;

    nn::Result result = nn::fs::OpenFile(
        &handle, path, nn::fs::FileOpenMode::FileOpenMode_Readable);

    // TODO: file not found
    if (!result.Ok()) {
        return result;
    }

    result = nn::fs::GetFileSize(data_size, handle);
    if (!result.Ok()) {
        return result;
    }

    *data = malloc(*data_size);
    if (!*data) {
        result.SetValue(0xBEEF);
        return result;
    }

    result = nn::fs::ReadFile(handle, 0, *data, *data_size);
    if (!result.Ok()) {
        return result;
    }

    nn::fs::CloseFile(handle);

    return result;
}

void *nv_alloc(size_t size, size_t alignment, void *unk) {
    return aligned_alloc(alignment, (size + alignment - 1) & -alignment);
}

void nv_free(void *ptr, void *unk) { free(ptr); }

void *nv_realloc(void *ptr, size_t size, void *unk) {
    return realloc(ptr, size);
}

void init_nv(void) {
    nv::SetGraphicsAllocator(nv_alloc, nv_free, nv_realloc, nullptr);
    nv::SetGraphicsDevtoolsAllocator(nv_alloc, nv_free, nv_realloc, nullptr);

    // This will never be free as it is entirely unmapped from the current
    // process by the nv service.
    size_t graphicsMemorySize = 0x10000000;  // 256MB
    void *graphicsMemory = nv_alloc(graphicsMemorySize, 0x1000, nullptr);

    debug_log("graphicsMemory: %p", graphicsMemory);

    if (graphicsMemory == nullptr) {
        nn::Result result(0xDEAD);
        gloat::utils::Abort(&result);
    }

    nv::InitializeGraphics(graphicsMemory, graphicsMemorySize);
}

GLuint loadDDS(const char *path);

void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *userParam) {
    debug_log("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
              (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
              severity, message);
}

//-----------------------------------------------------------------------------
// EGL initialization
//-----------------------------------------------------------------------------

static EGLDisplay s_display;
static EGLContext s_context;
static EGLSurface s_surface;

extern bool g_opengl_core;
extern int g_opengl_major;
extern int g_opengl_minor;

static bool initEgl() {
    nn::Result result(0);

    nn::vi::Initialize();

    nn::vi::Display *display;
    nn::vi::Layer *layer;

    result = nn::vi::OpenDefaultDisplay(&display);

    if (!result.Ok()) {
        debug_log("Could not open default display! error: %08x",
                  result.GetValue());
        goto _fail0;
    }

    result = nn::vi::CreateLayer(&layer, display, 1280, 720);
    if (!result.Ok()) {
        debug_log("Could not create layer! error: %08x", result.GetValue());
        goto _fail0;
    }

    // result = nn::vi::SetLayerScalingMode(layer, nn::vi::ScalingMode::Unk0);
    // result = nn::vi::SetLayerScalingMode(layer, nn::vi::ScalingMode::Unk2);
    if (!result.Ok()) {
        debug_log("Could not set layer scaling mode! error: %08x",
                  result.GetValue());
        goto _fail0;
    }

    void *nativeWindow;

    result = nn::vi::GetNativeWindow(&nativeWindow, layer);
    if (!result.Ok()) {
        debug_log("Could not get native window! error: %08x",
                  result.GetValue());
        goto _fail0;
    }

    // Connect to the EGL default display
    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!s_display) {
        debug_log("Could not connect to display! error: %d", eglGetError());
        goto _fail0;
    }

    // Initialize the EGL display connection
    eglInitialize(s_display, nullptr, nullptr);

    // Select OpenGL (Core) as the desired graphics API
    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
        debug_log("Could not set API! error: %d", eglGetError());
        goto _fail1;
    }

    // Get an appropriate EGL framebuffer configuration
    EGLConfig config;
    EGLint numConfigs;
    static const EGLint framebufferAttributeList[] = {EGL_RENDERABLE_TYPE,
                                                      EGL_OPENGL_BIT,
                                                      EGL_RED_SIZE,
                                                      8,
                                                      EGL_GREEN_SIZE,
                                                      8,
                                                      EGL_BLUE_SIZE,
                                                      8,
                                                      EGL_NONE};
    eglChooseConfig(s_display, framebufferAttributeList, &config, 1,
                    &numConfigs);
    if (numConfigs == 0) {
        debug_log("No config found! error: %d", eglGetError());
        goto _fail1;
    }

    // Create an EGL window surface
    s_surface =
        eglCreateWindowSurface(s_display, config, nativeWindow, nullptr);
    if (!s_surface) {
        debug_log("Surface creation failed! error: %d", eglGetError());
        goto _fail1;
    }

    {
        const EGLint profile{
            g_opengl_core ? EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR
                          : EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR};

        // Create an EGL rendering context
        const EGLint contextAttributeList[] = {
            EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,
            profile,
            EGL_CONTEXT_MAJOR_VERSION_KHR,
            g_opengl_major,
            EGL_CONTEXT_MINOR_VERSION_KHR,
            g_opengl_minor,
            EGL_NONE};
        s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT,
                                     contextAttributeList);
        if (!s_context) {
            debug_log("Context creation failed! error: %d", eglGetError());
            goto _fail2;
        }

        // Connect the context to the surface
        eglMakeCurrent(s_display, s_surface, s_surface, s_context);
        return true;
    }

_fail2:
    eglDestroySurface(s_display, s_surface);
    s_surface = nullptr;
_fail1:
    eglTerminate(s_display);
    s_display = nullptr;
_fail0:
    gloat::utils::Abort(&result);
    return false;
}

static void deinitEgl() {
    if (s_display) {
        eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT);
        if (s_context) {
            eglDestroyContext(s_display, s_context);
            s_context = nullptr;
        }
        if (s_surface) {
            eglDestroySurface(s_display, s_surface);
            s_surface = nullptr;
        }
        eglTerminate(s_display);
        s_display = nullptr;
    }
}

void init();
void render();
void quit();
void chooseLayer(int layerId);
int getLayer();

nn::hid::KeyboardState g_KeyboardState;

void updateHid() {
    memset(&g_KeyboardState, 0, sizeof(g_KeyboardState));
    nn::hid::GetKeyboardState(&g_KeyboardState);
}

extern "C"  void nnosSleepThread(uint64_t nano);

extern "C" void nnMain(void) {
    debug_log("Hello from nnMain :)");
    init_fs();
    init_nv();
    nn::hid::InitializeKeyboard();

    nn::Result result(0);

    if (!initEgl()) return;
    if (!gladLoadGL()) return;

    init();

    // TODO: wait for vsync
    while (true) {
        updateHid();

        bool is_a_pressed = !!(g_KeyboardState.keys[0] & (1 << 0x04));
        chooseLayer(!is_a_pressed);
        render();
        eglSwapBuffers(s_display, s_surface);
    }

    quit();
    deinitEgl();

    // NEEDED
    while (true) {
    }
}