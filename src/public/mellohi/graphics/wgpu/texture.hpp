#pragma once

#include "mellohi/graphics/wgpu/surface.hpp"

namespace mellohi::wgpu
{
    class Texture
    {
    public:
        explicit Texture(const Surface &surface);
        ~Texture();
        Texture(const Texture &other);
        Texture(Texture &&other) noexcept;
        Texture & operator=(const Texture &other);
        Texture & operator=(Texture &&other) noexcept;

        auto get_dimension() const -> WGPUTextureDimension;
        auto get_format() const -> WGPUTextureFormat;
        auto get_size() const -> vec2u;
        auto get_mip_level_count() const -> u32;
        auto get_sample_count() const -> u32;
        auto get_label() const -> const string &;

        auto get_raw_ptr() const -> WGPUTexture;

    private:
        WGPUTexture m_wgpu_texture{nullptr};
        string m_label{};
    };
}
