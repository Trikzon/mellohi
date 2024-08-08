#include "mellohi/graphics/wgpu/surface.hpp"

#include <glfw3webgpu.h>

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    Surface::Surface(const Instance &instance, const glfw::Window &window)
    {
        m_handle = std::make_shared<Handle>();

        m_handle->wgpu_surface = glfwGetWGPUSurface(instance.get_raw_ptr(), window.get_raw_ptr());
    }

    auto Surface::configure(const Device &device, const vec2u size) const -> void
    {
        const WGPUSurfaceConfiguration surface_configuration
        {
            .nextInChain = nullptr,
            .device = device.get_raw_ptr(),
            // TODO: Move to a constant.
            .format = WGPUTextureFormat_BGRA8Unorm,
            .usage = WGPUTextureUsage_RenderAttachment,
            .viewFormatCount = 0,
            .viewFormats = nullptr,
            .alphaMode = WGPUCompositeAlphaMode_Auto,
            .width = size.x,
            .height = size.y,
            .presentMode = m_handle->vsync ? WGPUPresentMode_Fifo : WGPUPresentMode_Immediate,
        };

        wgpuSurfaceConfigure(m_handle->wgpu_surface, &surface_configuration);

        m_handle->dirty = false;
    }

    auto Surface::present() const -> void
    {
        wgpuSurfacePresent(m_handle->wgpu_surface);
    }

    auto Surface::get_clear_color() const -> vec3f
    {
        return m_handle->clear_color;
    }

    auto Surface::set_clear_color(const vec3f clear_color) const -> void
    {
        m_handle->clear_color = clear_color;
    }

    auto Surface::get_vsync() const -> bool
    {
        return m_handle->vsync;
    }

    auto Surface::set_vsync(const bool vsync) const -> void
    {
        m_handle->vsync = vsync;
        m_handle->dirty = true;
    }

    auto Surface::is_dirty() const -> bool
    {
        return m_handle->dirty;
    }

    auto Surface::get_raw_ptr() const -> WGPUSurface
    {
        return m_handle->wgpu_surface;
    }

    Surface::Handle::~Handle()
    {
        if (wgpu_surface != nullptr)
        {
            wgpuSurfaceRelease(wgpu_surface);
        }
    }
}
