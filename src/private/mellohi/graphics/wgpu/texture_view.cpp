#include "mellohi/graphics/wgpu/texture_view.hpp"

namespace mellohi::wgpu
{
    TextureView::TextureView(const Texture &texture)
    {
        const string label = texture.get_label() + " View";

        WGPUTextureViewDimension dimension;
        switch (texture.get_dimension())
        {
            case WGPUTextureDimension_1D:
                dimension = WGPUTextureViewDimension_1D;
                break;
            case WGPUTextureDimension_2D:
                dimension = WGPUTextureViewDimension_2D;
                break;
            case WGPUTextureDimension_3D:
                dimension = WGPUTextureViewDimension_3D;
                break;
            default:
                MH_ASSERT(false, "Invalid texture dimension.");
        }

        const WGPUTextureViewDescriptor texture_view_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
            .format = texture.get_format(),
            .dimension = dimension,
            .baseMipLevel = 0,
            .mipLevelCount = texture.get_mip_level_count(),
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
            .aspect = WGPUTextureAspect_All
        };

        m_wgpu_texture_view = wgpuTextureCreateView(texture.get_raw_ptr(), &texture_view_descriptor);
    }

    TextureView::~TextureView()
    {
        if (m_wgpu_texture_view != nullptr)
        {
            wgpuTextureViewRelease(m_wgpu_texture_view);
            m_wgpu_texture_view = nullptr;
        }
    }

    TextureView::TextureView(const TextureView &other)
    {
        m_wgpu_texture_view = other.m_wgpu_texture_view;
        if (m_wgpu_texture_view != nullptr)
        {
            wgpuTextureViewAddRef(m_wgpu_texture_view);
        }
    }

    TextureView::TextureView(TextureView &&other) noexcept
    {
        std::swap(other.m_wgpu_texture_view, m_wgpu_texture_view);
    }

    auto TextureView::operator=(const TextureView &other) -> TextureView &
    {
        if (this != &other)
        {
            if (m_wgpu_texture_view != nullptr)
            {
                wgpuTextureViewRelease(m_wgpu_texture_view);
            }

            m_wgpu_texture_view = other.m_wgpu_texture_view;
            if (m_wgpu_texture_view != nullptr)
            {
                wgpuTextureViewAddRef(m_wgpu_texture_view);
            }
        }

        return *this;
    }

    auto TextureView::operator=(TextureView &&other) noexcept -> TextureView &
    {
        if (this != &other)
        {
            std::swap(other.m_wgpu_texture_view, m_wgpu_texture_view);
        }

        return *this;
    }

    auto TextureView::get_raw_ptr() const -> WGPUTextureView
    {
        return m_wgpu_texture_view;
    }
}
