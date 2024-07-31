#pragma once

#include "mellohi/graphics/glfw/window.hpp"
#include "mellohi/graphics/wgpu/instance.hpp"

namespace mellohi::wgpu
{
    class Device;

    class Surface
    {
    public:
        Surface(const Instance &instance, const glfw::Window &window);
        ~Surface();
        Surface(const Surface &other);
        Surface(Surface &&other) noexcept;
        auto operator=(const Surface &other) -> Surface &;
        auto operator=(Surface &&other) noexcept -> Surface &;

        auto configure(const Device &device, vec2u size, bool vsync) const -> void;
        auto present() const -> void;

        auto get_raw_ptr() const -> WGPUSurface;

    private:
        WGPUSurface m_wgpu_surface{nullptr};
    };
}
