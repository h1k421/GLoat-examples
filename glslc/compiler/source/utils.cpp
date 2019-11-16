#include <string>
#include <vector>

void fs_free(void *ptr, size_t unk)  // unk = size?
{
    free(ptr);
}

void *nv_alloc(size_t size, size_t alignment, void *unk) {
    return aligned_alloc(alignment, (size + alignment - 1) & -alignment);
}

void nv_free(void *ptr, void *unk) { free(ptr); }

void *nv_realloc(void *ptr, size_t size, void *unk) {
    return realloc(ptr, size);
}