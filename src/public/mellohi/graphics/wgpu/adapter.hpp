#pragma once

#include "mellohi/graphics/wgpu/surface.hpp"

namespace mellohi::wgpu
{
    class Adapter
    {
    public:
        Adapter(const Instance &instance, const Surface &surface);
        ~Adapter();
        Adapter(const Adapter &other);
        Adapter(Adapter &&other) noexcept;
        auto operator=(const Adapter &other) -> Adapter &;
        auto operator=(Adapter &&other) noexcept -> Adapter &;

        auto get_limits() const -> WGPULimits;

        auto get_raw_ptr() const -> WGPUAdapter;

    private:
        WGPUAdapter m_wgpu_adapter{nullptr};
    };
}
