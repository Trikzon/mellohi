#pragma once

#include "mellohi/graphics/wgpu/sampler.hpp"
#include "mellohi/graphics/wgpu/texture_view.hpp"
#include "mellohi/graphics/wgpu/uniform_buffer.hpp"

namespace mellohi::wgpu
{
    class BindGroup
    {
    public:
        BindGroup(const Device &device, const string &label, u32 initial_dynamic_size = INITIAL_DYNAMIC_SIZE);

        auto add_binding(u32 binding_idx, u32 size_bytes) const -> void;
        auto add_binding(u32 binding_idx, const TextureView &texture_view) const -> void;
        auto add_binding(u32 binding_idx, const Sampler &sampler) const -> void;
        auto write(u32 binding_idx, u32 dynamic_idx, const void *data, u32 size_bytes) const -> void;

        auto get_dynamic_offsets(u32 dynamic_idx) const -> vector<u32>;

        auto get_raw_ptr() const -> WGPUBindGroup;
        auto get_raw_layout_ptr() const -> WGPUBindGroupLayout;

    private:
        constexpr static u32 INITIAL_DYNAMIC_SIZE = 3;

        struct Handle
        {
            s_ptr<Device> device{nullptr};
            string label{};
            WGPUBindGroup wgpu_bind_group{nullptr};
            WGPUBindGroupLayout wgpu_bind_group_layout{nullptr};
            vector<UniformBuffer> uniform_buffers{};
            vector<WGPUBindGroupEntry> wgpu_entries{};
            vector<WGPUBindGroupLayoutEntry> wgpu_layout_entries{};
            u32 dynamic_size{INITIAL_DYNAMIC_SIZE};

            Handle() = default;
            ~Handle();
            Handle(const Handle &handle) = delete;
            Handle(Handle &&handle) = delete;
            Handle & operator=(const Handle &handle) = delete;
            Handle & operator=(Handle &&handle) = delete;

            auto rebuild() -> void;
            auto grow_dynamic_size(u32 new_dynamic_size) -> void;
        };

        s_ptr<Handle> m_handle{nullptr};
    };
}
