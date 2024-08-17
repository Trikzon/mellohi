#include "mellohi/graphics/wgpu/texture.hpp"

#include <bit>

#include <stb/image.h>

#include "mellohi/core/log.hpp"
#include "mellohi/graphics/wgpu/queue.hpp"

namespace mellohi::wgpu
{
    Texture::Texture(const Device& device, const AssetId &asset_id)
    {
        i32 width, height, channels;
        u8 *pixels = stbi_load(asset_id.get_file_path().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        MH_ASSERT(pixels != nullptr, "Failed to load texture from: {}", asset_id);
        MH_ASSERT(width > 0 && height > 0, "Invalid texture size: {}x{}", width, height);

        m_label = asset_id.get_fully_qualified_id() + " Texture";

        const WGPUTextureDescriptor texture_descriptor
        {
             .nextInChain = nullptr,
             .label = m_label.c_str(),
             .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
             .dimension = WGPUTextureDimension_2D,
             .size = { static_cast<u32>(width), static_cast<u32>(height), 1 },
             .format = WGPUTextureFormat_RGBA8Unorm,
             .mipLevelCount = static_cast<u32>(std::bit_width(static_cast<u32>(std::max(width, height)))),
             .sampleCount = 1,
             .viewFormatCount = 0,
             .viewFormats = nullptr,
        };

        m_wgpu_texture = wgpuDeviceCreateTexture(device.get_raw_ptr(), &texture_descriptor);

        write_mip_maps(device, texture_descriptor.size, texture_descriptor.mipLevelCount, pixels);

        stbi_image_free(pixels);
    }

    Texture::Texture(const Surface &surface) : m_label{"Mellohi Current Surface Texture"}
    {
        WGPUSurfaceTexture surface_texture{};
        wgpuSurfaceGetCurrentTexture(surface.get_raw_ptr(), &surface_texture);

        MH_ASSERT(surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_Success,
                  "Failed to get current texture from surface.");

        m_wgpu_texture = surface_texture.texture;
        wgpuTextureSetLabel(m_wgpu_texture, m_label.c_str());
    }

    Texture::~Texture()
    {
        if (m_wgpu_texture != nullptr)
        {
            wgpuTextureRelease(m_wgpu_texture);
            m_wgpu_texture = nullptr;

            m_label = "Released " + m_label;
        }
    }

    Texture::Texture(const Texture &other)
    {
        m_wgpu_texture = other.m_wgpu_texture;
        if (m_wgpu_texture != nullptr)
        {
            wgpuTextureAddRef(m_wgpu_texture);
        }

        m_label = other.m_label;
    }

    Texture::Texture(Texture &&other) noexcept
    {
        std::swap(other.m_wgpu_texture, m_wgpu_texture);
        std::swap(other.m_label, m_label);
    }

    Texture & Texture::operator=(const Texture &other)
    {
        if (this != &other)
        {
            if (m_wgpu_texture != nullptr)
            {
                wgpuTextureRelease(m_wgpu_texture);

                m_label = "Released " + m_label;
            }

            m_wgpu_texture = other.m_wgpu_texture;
            if (m_wgpu_texture != nullptr)
            {
                wgpuTextureAddRef(m_wgpu_texture);
            }

            m_label = other.m_label;
        }

        return *this;
    }

    Texture & Texture::operator=(Texture &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(other.m_wgpu_texture, m_wgpu_texture);
            std::swap(other.m_label, m_label);
        }

        return *this;
    }

    auto Texture::get_dimension() const -> WGPUTextureDimension
    {
        return wgpuTextureGetDimension(m_wgpu_texture);
    }

    auto Texture::get_format() const -> WGPUTextureFormat
    {
        return wgpuTextureGetFormat(m_wgpu_texture);
    }

    auto Texture::get_size() const -> vec2u
    {
        usize width = wgpuTextureGetWidth(m_wgpu_texture);
        usize height = wgpuTextureGetHeight(m_wgpu_texture);
        return {width, height};
    }

    auto Texture::get_mip_level_count() const -> u32
    {
        return wgpuTextureGetMipLevelCount(m_wgpu_texture);
    }

    auto Texture::get_sample_count() const -> u32
    {
        return wgpuTextureGetSampleCount(m_wgpu_texture);
    }

    auto Texture::get_label() const -> const string &
    {
        return m_label;
    }

    auto Texture::get_raw_ptr() const -> WGPUTexture
    {
        return m_wgpu_texture;
    }

    auto Texture::write_mip_maps(const Device &device, const WGPUExtent3D texture_size, const u32 mip_level_count,
                                 const u8 *pixels) const -> void
    {
        const Queue queue{device};

        WGPUImageCopyTexture destination
        {
            .texture = m_wgpu_texture,
            .origin = { 0, 0, 0 },
            .aspect = WGPUTextureAspect_All,
        };

        WGPUTextureDataLayout source
        {
            .offset = 0,
        };

        WGPUExtent3D mip_level_size = texture_size;
        vector<u8> prev_mip_level_pixels;
        WGPUExtent3D prev_mip_level_size = texture_size;
        for (auto mip_level = 0; mip_level < mip_level_count; ++mip_level)
        {
            vector<u8> mip_level_pixels(4 * mip_level_size.width * mip_level_size.height);
            if (mip_level == 0)
            {
                memcpy(mip_level_pixels.data(), pixels, mip_level_pixels.size());
            }
            else
            {
                for (auto i = 0; i < mip_level_size.width; ++i)
                {
                    for (auto j = 0; j < mip_level_size.height; ++j)
                    {
                        u8 *p = &mip_level_pixels[4 * (j * mip_level_size.width + i)];
                        // Get the corresponding 4 pixels from the previous level
                        const unsigned char* p00 = &prev_mip_level_pixels[4 * ((2 * j + 0) * prev_mip_level_size.width + (2 * i + 0))];
                        const unsigned char* p01 = &prev_mip_level_pixels[4 * ((2 * j + 0) * prev_mip_level_size.width + (2 * i + 1))];
                        const unsigned char* p10 = &prev_mip_level_pixels[4 * ((2 * j + 1) * prev_mip_level_size.width + (2 * i + 0))];
                        const unsigned char* p11 = &prev_mip_level_pixels[4 * ((2 * j + 1) * prev_mip_level_size.width + (2 * i + 1))];
                        // Average
                        p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / 4;
                        p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / 4;
                        p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / 4;
                        p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / 4;
                    }
                }
            }

            destination.mipLevel = mip_level;
            source.bytesPerRow = 4 * mip_level_size.width;
            source.rowsPerImage = mip_level_size.height;

            wgpuQueueWriteTexture(queue.get_raw_ptr(), &destination, mip_level_pixels.data(), mip_level_pixels.size(),
                                  &source, &mip_level_size);

            prev_mip_level_pixels = std::move(mip_level_pixels);
            prev_mip_level_size = mip_level_size;
            // mip_level_size = {std::max(1u, mip_level_size.width / 2), std::max(1u, mip_level_size.height / 2)};
            mip_level_size.width /= 2;
            mip_level_size.height /= 2;
        }
    }
}
