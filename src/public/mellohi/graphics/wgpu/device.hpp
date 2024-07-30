#pragma once

#include "mellohi/graphics/wgpu/adapter.hpp"

namespace mellohi::wgpu
{
    class Device
    {
    public:
        Device(const Adapter &adapter, const WGPULimits &limits);
        ~Device();
        Device(const Device &other);
        Device(Device &&other) noexcept;
        auto operator=(const Device &other) -> Device &;
        auto operator=(Device &&other) noexcept -> Device &;

        auto tick() const -> void;

        auto get_limits() const -> WGPULimits;

        auto get_raw_ptr() const -> WGPUDevice;

    private:
        WGPUDevice m_wgpu_device{nullptr};
    };
}
