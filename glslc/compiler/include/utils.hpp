#pragma once

void fs_free(void *ptr, size_t unk);
void *nv_alloc(size_t size, size_t alignment, void *unk);
void nv_free(void *ptr, void *unk);
void *nv_realloc(void *ptr, size_t size, void *unk);