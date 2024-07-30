#pragma once

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    class UniformBuffer
    {
    public:
        UniformBuffer(const Device &device, const string &label, u32 binding_idx, u32 dynamic_size, u64 size_bytes);
        ~UniformBuffer();
        UniformBuffer(const UniformBuffer &other);
        UniformBuffer(UniformBuffer &&other) noexcept;
        auto operator=(const UniformBuffer &other) -> UniformBuffer &;
        auto operator=(UniformBuffer &&other) noexcept -> UniformBuffer &;

        auto get_binding_idx() const -> u32;
        auto get_size_bytes() const -> u64;
        auto get_stride_bytes() const -> u64;

        auto get_wgpu_entry() const -> const WGPUBindGroupEntry &;
        auto get_wgpu_layout() const -> const WGPUBindGroupLayoutEntry &;

        auto get_raw_ptr() const -> WGPUBuffer;

    private:
        WGPUBuffer m_wgpu_buffer{nullptr};
        WGPUBindGroupEntry m_wgpu_entry{};
        WGPUBindGroupLayoutEntry m_wgpu_layout{};
        u32 m_binding_idx{0};
        u64 m_size_bytes{0};
        u64 m_stride_bytes{0};
    };
}
