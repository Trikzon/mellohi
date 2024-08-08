#include "mellohi/graphics/wgpu/surface.hpp"

#include <glfw3webgpu.h>

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    Surface::Surface(const Instance &instance, const glfw::Window &window)
    {
        m_wgpu_surface = glfwGetWGPUSurface(instance.get_raw_ptr(), window.get_raw_ptr());
    }

    Surface::~Surface()
    {
        if (m_wgpu_surface != nullptr)
        {
            wgpuSurfaceRelease(m_wgpu_surface);
            m_wgpu_surface = nullptr;
        }
    }

    Surface::Surface(const Surface &other)
    {
        m_wgpu_surface = other.m_wgpu_surface;
        if (m_wgpu_surface != nullptr)
        {
            wgpuSurfaceAddRef(m_wgpu_surface);
        }
    }

    Surface::Surface(Surface &&other) noexcept
    {
        std::swap(other.m_wgpu_surface, m_wgpu_surface);
    }

    auto Surface::operator=(const Surface &other) -> Surface &
    {
        if (this != &other)
        {
            if (m_wgpu_surface != nullptr)
            {
                wgpuSurfaceRelease(m_wgpu_surface);
            }

            m_wgpu_surface = other.m_wgpu_surface;
            if (m_wgpu_surface != nullptr)
            {
                wgpuSurfaceAddRef(m_wgpu_surface);
            }
        }

        return *this;
    }

    auto Surface::operator=(Surface &&other) noexcept -> Surface &
    {
        if (this != &other)
        {
            std::swap(other.m_wgpu_surface, m_wgpu_surface);
        }

        return *this;
    }

    auto Surface::configure(const Device &device, const vec2u size, const bool vsync) const -> void
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
            .presentMode = vsync ? WGPUPresentMode_Fifo : WGPUPresentMode_Immediate,
        };

        wgpuSurfaceConfigure(m_wgpu_surface, &surface_configuration);
    }

    auto Surface::present() const -> void
    {
        wgpuSurfacePresent(m_wgpu_surface);
    }

    auto Surface::get_raw_ptr() const -> WGPUSurface
    {
        return m_wgpu_surface;
    }
}
