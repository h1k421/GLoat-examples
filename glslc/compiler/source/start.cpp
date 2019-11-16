#include <gloat.hpp>
#include <string>
#include <vector>

#include "utils.hpp"

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

extern "C" void appMain();

extern "C" WEAK size_t __gloat_get_heap_size() {
    return 0x1000000 * 4;
}

extern "C" void nnMain(void) {

    init_fs();

    appMain();
    // NEEDED
    while (true) {}
}