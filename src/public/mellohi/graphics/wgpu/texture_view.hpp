#pragma once

#include "mellohi/graphics/wgpu/texture.hpp"

namespace mellohi::wgpu
{
    class TextureView
    {
    public:
        explicit TextureView(const Texture &texture);
        ~TextureView();
        TextureView(const TextureView &other);
        TextureView(TextureView &&other) noexcept;
        auto operator=(const TextureView &other) -> TextureView &;
        auto operator=(TextureView &&other) noexcept -> TextureView &;

        auto get_raw_ptr() const -> WGPUTextureView;

    private:
        WGPUTextureView m_wgpu_texture_view{nullptr};
    };
}
