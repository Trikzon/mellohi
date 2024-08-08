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

        auto configure(const Device &device, vec2u size) const -> void;
        auto present() const -> void;

        auto get_clear_color() const -> vec3f;
        auto set_clear_color(vec3f clear_color) const -> void;

        auto get_vsync() const -> bool;
        auto set_vsync(bool vsync) const -> void;

        auto is_dirty() const -> bool;

        auto get_raw_ptr() const -> WGPUSurface;

    private:
        struct Handle
        {
            vec3f clear_color{0.1f, 0.05f, 0.1f};
            bool vsync{false}, dirty{false};
            WGPUSurface wgpu_surface{nullptr};

            ~Handle();
        };

        s_ptr<Handle> m_handle{nullptr};
    };
}
