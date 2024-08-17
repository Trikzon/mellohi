#pragma once

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    class Sampler
    {
    public:
        Sampler(const Device &device, const string &label, bool linearly_filter);
        ~Sampler();
        Sampler(const Sampler &other);
        Sampler(Sampler &&other) noexcept;
        auto operator=(const Sampler &other) -> Sampler &;
        auto operator=(Sampler &&other) noexcept -> Sampler &;

        auto get_raw_ptr() const -> WGPUSampler;

    private:
        WGPUSampler m_wgpu_sampler{nullptr};
    };
}
