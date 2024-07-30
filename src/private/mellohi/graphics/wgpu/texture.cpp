#include "mellohi/graphics/wgpu/texture.hpp"

namespace mellohi::wgpu
{
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
}
