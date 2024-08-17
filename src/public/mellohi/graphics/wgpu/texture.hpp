#pragma once

#include "mellohi/core/asset_id.hpp"
#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    class Texture
    {
    public:
        Texture(const Device& device, const AssetId &asset_id);
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

        auto write_mip_maps(const Device &device, WGPUExtent3D texture_size, u32 mip_level_count,
                            const u8 *pixels) const -> void;
    };
}
