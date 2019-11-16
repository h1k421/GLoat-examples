#include <algorithm>
#include <gloat.hpp>
#include "GL/glcorearb.h"

// DDS_Flags
#define DDSD_CAPS 0x1
#define DDSD_HEIGHT 0x2
#define DDSD_WIDTH 0x4
#define DDSD_PITCH 0x8
#define DDSD_PIXELFORMAT 0x1000
#define DDSD_MIPMAPCOUNT 0x20000
#define DDSD_LINEARSIZE 0x80000
#define DDSD_DEPTH 0x800000

// DDS_Cap1
#define DDSCAPS_COMPLEX 0x8
#define DDSCAPS_MIPMAP 0x400000
#define DDSCAPS_TEXTURE 0x1000

// FourCC
#define FOURCC_DXT1 "DXT1"
#define FOURCC_DXT3 "DXT3"
#define FOURCC_DXT5 "DXT5"
#define FOURCC_DX10 "DX10"
#define FOURCC_ATI2 "ATI2"

// Pixel Format Flags
#define DDPF_ALPHAPIXELS 0x1
#define DDPF_FOURCC 0x4
#define DDPF_RGB 0x40

typedef enum {
    D3D10_RESOURCE_DIMENSION_UNKNOWN,
    D3D10_RESOURCE_DIMENSION_BUFFER,
    D3D10_RESOURCE_DIMENSION_TEXTURE1D,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D
} d3d10_resource_dimension;

typedef enum {
    DXGI_FORMAT_UNKNOWN,
    DXGI_FORMAT_R32G32B32A32_TYPELESS,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_R32G32B32_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32_UINT,
    DXGI_FORMAT_R32G32B32_SINT,
    DXGI_FORMAT_R16G16B16A16_TYPELESS,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_UNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_R32G32_TYPELESS,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32_UINT,
    DXGI_FORMAT_R32G32_SINT,
    DXGI_FORMAT_R32G8X24_TYPELESS,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
    DXGI_FORMAT_R10G10B10A2_TYPELESS,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R10G10B10A2_UINT,
    DXGI_FORMAT_R11G11B10_FLOAT,
    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_UINT,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R8G8B8A8_SINT,
    DXGI_FORMAT_R16G16_TYPELESS,
    DXGI_FORMAT_R16G16_FLOAT,
    DXGI_FORMAT_R16G16_UNORM,
    DXGI_FORMAT_R16G16_UINT,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_R16G16_SINT,
    DXGI_FORMAT_R32_TYPELESS,
    DXGI_FORMAT_D32_FLOAT,
    DXGI_FORMAT_R32_FLOAT,
    DXGI_FORMAT_R32_UINT,
    DXGI_FORMAT_R32_SINT,
    DXGI_FORMAT_R24G8_TYPELESS,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT,
    DXGI_FORMAT_R8G8_TYPELESS,
    DXGI_FORMAT_R8G8_UNORM,
    DXGI_FORMAT_R8G8_UINT,
    DXGI_FORMAT_R8G8_SNORM,
    DXGI_FORMAT_R8G8_SINT,
    DXGI_FORMAT_R16_TYPELESS,
    DXGI_FORMAT_R16_FLOAT,
    DXGI_FORMAT_D16_UNORM,
    DXGI_FORMAT_R16_UNORM,
    DXGI_FORMAT_R16_UINT,
    DXGI_FORMAT_R16_SNORM,
    DXGI_FORMAT_R16_SINT,
    DXGI_FORMAT_R8_TYPELESS,
    DXGI_FORMAT_R8_UNORM,
    DXGI_FORMAT_R8_UINT,
    DXGI_FORMAT_R8_SNORM,
    DXGI_FORMAT_R8_SINT,
    DXGI_FORMAT_A8_UNORM,
    DXGI_FORMAT_R1_UNORM,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
    DXGI_FORMAT_R8G8_B8G8_UNORM,
    DXGI_FORMAT_G8R8_G8B8_UNORM,
    DXGI_FORMAT_BC1_TYPELESS,
    DXGI_FORMAT_BC1_UNORM,
    DXGI_FORMAT_BC1_UNORM_SRGB,
    DXGI_FORMAT_BC2_TYPELESS,
    DXGI_FORMAT_BC2_UNORM,
    DXGI_FORMAT_BC2_UNORM_SRGB,
    DXGI_FORMAT_BC3_TYPELESS,
    DXGI_FORMAT_BC3_UNORM,
    DXGI_FORMAT_BC3_UNORM_SRGB,
    DXGI_FORMAT_BC4_TYPELESS,
    DXGI_FORMAT_BC4_UNORM,
    DXGI_FORMAT_BC4_SNORM,
    DXGI_FORMAT_BC5_TYPELESS,
    DXGI_FORMAT_BC5_UNORM,
    DXGI_FORMAT_BC5_SNORM,
    DXGI_FORMAT_B5G6R5_UNORM,
    DXGI_FORMAT_B5G5R5A1_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
    DXGI_FORMAT_B8G8R8A8_TYPELESS,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
    DXGI_FORMAT_B8G8R8X8_TYPELESS,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
    DXGI_FORMAT_BC6H_TYPELESS,
    DXGI_FORMAT_BC6H_UF16,
    DXGI_FORMAT_BC6H_SF16,
    DXGI_FORMAT_BC7_TYPELESS,
    DXGI_FORMAT_BC7_UNORM,
    DXGI_FORMAT_BC7_UNORM_SRGB,
    DXGI_FORMAT_AYUV,
    DXGI_FORMAT_Y410,
    DXGI_FORMAT_Y416,
    DXGI_FORMAT_NV12,
    DXGI_FORMAT_P010,
    DXGI_FORMAT_P016,
    DXGI_FORMAT_420_OPAQUE,
    DXGI_FORMAT_YUY2,
    DXGI_FORMAT_Y210,
    DXGI_FORMAT_Y216,
    DXGI_FORMAT_NV11,
    DXGI_FORMAT_AI44,
    DXGI_FORMAT_IA44,
    DXGI_FORMAT_P8,
    DXGI_FORMAT_A8P8,
    DXGI_FORMAT_B4G4R4A4_UNORM,
    DXGI_FORMAT_P208,
    DXGI_FORMAT_V208,
    DXGI_FORMAT_V408,
    DXGI_FORMAT_FORCE_UINT
} dxgi_format;

typedef struct {
    uint32_t size;
    uint32_t flags;
    char fourCC[4];
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
} dds_pixelformat;

typedef struct {
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitchOrLinearSize;
    uint32_t depth;
    uint32_t mipMapCount;
    uint32_t reserved1[11];
    dds_pixelformat ddspf;
    uint32_t caps;
    uint32_t caps2;
    uint32_t caps3;
    uint32_t caps4;
    uint32_t reserved2;
} dds_header;

typedef struct {
    dxgi_format dxgiFormat;
    d3d10_resource_dimension resourceDimension;
    uint32_t miscFlag;
    uint32_t arraySize;
    uint32_t miscFlags2;
} dds_header_dx10;

GLuint loadDDS(const char* path) {
    // Actual RGB data
    unsigned char* data;
    int data_size;

    GLuint internal_format = GL_RGB;
    GLuint format = GL_BGR;
    GLuint target = GL_TEXTURE_2D;
    GLuint texture;

    nn::fs::FileHandle handle;

    nn::Result result = nn::fs::OpenFile(
        &handle, path, nn::fs::FileOpenMode::FileOpenMode_Readable);
    if (!result.Ok()) {
        gloat::utils::Abort(&result);
        debug_log("Image could not be opened\n");
        return 0;
    }

    char magic[4];

    long offset = 0;

    result = nn::fs::ReadFile(handle, offset, magic, 4);
    offset += 4;

    if (memcmp(magic, "DDS ", 4)) {
        gloat::utils::Abort(&result);
        debug_log("Not a valid DDS file\n");
        nn::fs::CloseFile(handle);
        return 0;
    }

    dds_header header;
    int isCompressed = 0;
    int bpp = 0;

    result = nn::fs::ReadFile(handle, offset, &header, sizeof(dds_header));
    offset += sizeof(dds_header);

    if ((header.flags &
         (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT)) == 0) {
        debug_log("DDS header found but flags aren't valid!\n");
        nn::fs::CloseFile(handle);
        return 0;
    }

    if ((header.caps & DDSCAPS_TEXTURE) == 0) {
        debug_log("DDS header found but caps aren't valid!\n");
        nn::fs::CloseFile(handle);
        return 0;
    }

    // TODO: check for cube array/cube/volume

    // FIXME: checks for DDPF_FOURCC too
    if ((header.flags & DDSD_PITCH) == 0) {
        isCompressed = 1;
        if ((header.flags & DDSD_LINEARSIZE) == 0) {
            debug_log("DDS has no pitch and no linear size!\n");
            nn::fs::CloseFile(handle);
            return 0;
        }
    }

    if (header.mipMapCount != 1) {
        debug_log("MipMap texture not supported yet\n");
        nn::fs::CloseFile(handle);
        return 0;
    }

    if (isCompressed || (header.ddspf.flags & DDPF_FOURCC) != 0) {
        int blockSize = 16;

        if (!memcmp(header.ddspf.fourCC, FOURCC_DXT1, 4)) {
            blockSize = 8;
            internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        } else if (!memcmp(header.ddspf.fourCC, FOURCC_DXT3, 4)) {
            internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        } else if (!memcmp(header.ddspf.fourCC, FOURCC_DXT5, 4)) {
            internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        } else if (!memcmp(header.ddspf.fourCC, FOURCC_ATI2, 4)) {
            internal_format = GL_COMPRESSED_RG_RGTC2;
        } else if (!memcmp(header.ddspf.fourCC, FOURCC_DX10, 4)) {
            dds_header_dx10 header_dx10;

            result = nn::fs::ReadFile(handle, offset, &header_dx10,
                                      sizeof(dds_header_dx10));
            offset += sizeof(dds_header_dx10);

            int isArray = header_dx10.arraySize > 1;

            switch (header_dx10.resourceDimension) {
                case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
                    target = isArray ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
                    break;
                case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
                    target = isArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
                    break;
                case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
                    target = GL_TEXTURE_3D;
                    break;
                default:
                    debug_log("Unsupported Resource Dimension 0x%x",
                              header_dx10.resourceDimension);
                    return 0;
            }

            switch (header_dx10.dxgiFormat) {
                case DXGI_FORMAT_R8G8B8A8_UNORM:
                    internal_format = GL_RGBA8;
                    format = GL_RGBA;
                    isCompressed = 0;
                    bpp = 4;

                    break;
                case DXGI_FORMAT_B8G8R8A8_UNORM:
                    internal_format = GL_RGBA8;
                    format = GL_BGRA;
                    isCompressed = 0;
                    bpp = 4;

                    break;
                case DXGI_FORMAT_BC3_UNORM:
                    internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                    isCompressed = 1;
                    bpp = 4;

                    break;
                default:
                    debug_log("Unsupported DXGI format 0x%x\n",
                              header_dx10.dxgiFormat);
                    return 0;
            }

            if (!isCompressed) {
                glGenTextures(1, &texture);

                // TODO: support depth
                glBindTexture(target, texture);

                data_size = header.pitchOrLinearSize * header.height *
                            header_dx10.arraySize;
                data = (unsigned char*)malloc(data_size);
                if (!data) {
                    nn::fs::CloseFile(handle);
                    return 0;
                }

                result = nn::fs::ReadFile(handle, offset, data, data_size);
                offset += data_size;
                nn::fs::CloseFile(handle);

                switch (target) {
                    case GL_TEXTURE_2D:
                        glTexImage2D(target, 0, internal_format, header.width,
                                     header.height, 0, format, GL_UNSIGNED_BYTE,
                                     data);
                        break;
                    case GL_TEXTURE_2D_ARRAY:
                        glTexImage3D(target, 0, internal_format, header.width,
                                     header.height, header_dx10.arraySize, 0,
                                     format, GL_UNSIGNED_BYTE, data);
                        // glGenerateMipmap(target);
                        // glTexStorage3D(target, 1, internal_format,
                        // header.width, header.height, header_dx10.arraySize);
                        // glTexSubImage3D(target, 0, 0, 0, 0, header.width,
                        // header.height, header_dx10.arraySize, format,
                        // GL_UNSIGNED_BYTE, data);
                        break;
                    default:
                        debug_log("Target not supported\n");
                        return 0;
                }

                goto end;
            }

            debug_log("DX10 not supported\n");
            return 0;
        } else {
            debug_log("Unsupported FourCC: \"%c%c%c%c\"",
                      header.ddspf.fourCC[0], header.ddspf.fourCC[1],
                      header.ddspf.fourCC[2], header.ddspf.fourCC[3]);
            nn::fs::CloseFile(handle);
            return 0;
        }

        header.pitchOrLinearSize =
            std::max(1u, ((header.width + 3) / 4)) * blockSize;

        data_size = header.pitchOrLinearSize * ((header.height + 3) / 4);
        data = (unsigned char*)malloc(data_size);

        if (!data) {
            nn::fs::CloseFile(handle);
            return 0;
        }

        result = nn::fs::ReadFile(handle, offset, data, data_size);
        offset += data_size;

        nn::fs::CloseFile(handle);

        glGenTextures(1, &texture);

        // TODO: support depth
        glBindTexture(target, texture);

        glCompressedTexImage2D(target, 0, internal_format, header.width,
                               header.height, GL_NONE, data_size, data);
    } else {
        if ((header.ddspf.flags & DDPF_RGB) != 0) {
            internal_format = GL_RGB;
            format = GL_BGR;
            bpp = 24;
        }

        if ((header.ddspf.flags & DDPF_ALPHAPIXELS) != 0) {
            if (bpp == 24) {
                internal_format = GL_RGBA;
                format = GL_BGRA;
            }
            bpp += 8;
        }

        if (bpp == 0) {
            debug_log("UNKNOWN UNCOMPRESSED FORMAT\n");
            nn::fs::CloseFile(handle);
            return 0;
        }

        // as advised by the MS documentation about DDS, we recompute the pitch.
        header.pitchOrLinearSize = (header.width * bpp + 7) / 8;
        debug_log("0x%x\n", header.pitchOrLinearSize);

        glGenTextures(1, &texture);

        // TODO: support depth
        glBindTexture(target, texture);

        data_size = header.pitchOrLinearSize * header.height;
        data = (unsigned char*)malloc(data_size);
        if (!data) {
            nn::fs::CloseFile(handle);
            return 0;
        }

        // read in the inverted order because direct X
        /*for (int Y = header.height - 1; Y >= 0; Y--)
        {
            fread(&data[Y * header.pitchOrLinearSize], 1,
        header.pitchOrLinearSize, file);
        }*/

        result = nn::fs::ReadFile(handle, offset, data, data_size);
        offset += data_size;
        nn::fs::CloseFile(handle);

        // Give the image to OpenGL
        glTexImage2D(target, 0, internal_format, header.width, header.height, 0,
                     format, GL_UNSIGNED_BYTE, data);
    }

end:
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // glGenerateMipmap(target);

    return texture;
}