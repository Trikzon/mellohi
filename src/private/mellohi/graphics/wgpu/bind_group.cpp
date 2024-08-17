#include "mellohi/graphics/wgpu/bind_group.hpp"

#include "mellohi/graphics/wgpu/queue.hpp"

namespace mellohi::wgpu
{
    BindGroup::BindGroup(const Device &device, const string &label, const u32 initial_dynamic_size)
    {
        m_handle = std::make_shared<Handle>();

        m_handle->device = std::make_shared<Device>(device);
        m_handle->label = label;
        m_handle->dynamic_size = initial_dynamic_size;

        m_handle->rebuild();
    }

    auto BindGroup::add_binding(const u32 binding_idx, const u32 size_bytes) const -> void
    {
        const auto label = m_handle->label + " Uniform Buffer " + std::to_string(binding_idx);

        m_handle->uniform_buffers.emplace_back(*m_handle->device, label, binding_idx, m_handle->dynamic_size,
                                               size_bytes);

        m_handle->rebuild();
    }

    auto BindGroup::add_binding(const u32 binding_idx, const TextureView &texture_view) const -> void
    {
        m_handle->wgpu_entries.push_back(WGPUBindGroupEntry{
            .nextInChain = nullptr,
            .binding = binding_idx,
            .buffer = nullptr,
            .offset = 0,
            .size = 0,
            .sampler = nullptr,
            .textureView = texture_view.get_raw_ptr(),
        });

        m_handle->wgpu_layout_entries.push_back(WGPUBindGroupLayoutEntry{
            .nextInChain = nullptr,
            .binding = binding_idx,
            .visibility = WGPUShaderStage_Fragment,
            .buffer = nullptr,
            .sampler = nullptr,
            .texture = WGPUTextureBindingLayout
            {
                .nextInChain = nullptr,
                .sampleType = WGPUTextureSampleType_Float,
                .viewDimension = WGPUTextureViewDimension_2D,
                .multisampled = false,
            },
            .storageTexture = nullptr,
        });

        m_handle->rebuild();
    }

    auto BindGroup::add_binding(const u32 binding_idx, const Sampler &sampler) const -> void
    {
        m_handle->wgpu_entries.push_back(WGPUBindGroupEntry{
            .nextInChain = nullptr,
            .binding = binding_idx,
            .buffer = nullptr,
            .offset = 0,
            .size = 0,
            .sampler = sampler.get_raw_ptr(),
            .textureView = nullptr,
        });

        m_handle->wgpu_layout_entries.push_back(WGPUBindGroupLayoutEntry{
            .nextInChain = nullptr,
            .binding = binding_idx,
            .visibility = WGPUShaderStage_Fragment,
            .buffer = nullptr,
            .sampler = WGPUSamplerBindingLayout
            {
                .nextInChain = nullptr,
                .type = WGPUSamplerBindingType_Filtering,
            },
            .texture = nullptr,
            .storageTexture = nullptr
        });

        m_handle->rebuild();
    }

    auto BindGroup::write(const u32 binding_idx, const u32 dynamic_idx, const void *data,
                          const u32 size_bytes) const -> void
    {
        if (dynamic_idx >= m_handle->dynamic_size)
        {
            u32 new_dynamic_size = static_cast<u32>(m_handle->dynamic_size * 1.5 + 1);
            if (dynamic_idx > new_dynamic_size)
            {
                new_dynamic_size = dynamic_idx;
            }

            m_handle->grow_dynamic_size(new_dynamic_size);
        }

        bool found = false;
        for (const auto &uniform_buffer: m_handle->uniform_buffers)
        {
            if (uniform_buffer.get_binding_idx() == binding_idx)
            {
                const Queue queue{*m_handle->device};
                queue.write_buffer(uniform_buffer, dynamic_idx, data, size_bytes);

                found = true;
                break;
            }
        }

        MH_ASSERT(found, "Uniform buffer with binding index {} not found in bind group {}.", binding_idx,
                  m_handle->label);
    }

    auto BindGroup::get_dynamic_offsets(const u32 dynamic_idx) const -> vector<u32>
    {
        if (dynamic_idx >= m_handle->dynamic_size)
        {
            u32 new_dynamic_size = static_cast<u32>(m_handle->dynamic_size * 1.5 + 1);
            if (dynamic_idx > new_dynamic_size)
            {
                new_dynamic_size = dynamic_idx;
            }

            m_handle->grow_dynamic_size(new_dynamic_size);
        }

        vector<u32> dynamic_offsets{};
        dynamic_offsets.reserve(m_handle->uniform_buffers.size());

        for (const auto &uniform_buffer: m_handle->uniform_buffers)
        {
            dynamic_offsets.push_back(dynamic_idx * uniform_buffer.get_stride_bytes());
        }

        return dynamic_offsets;
    }

    auto BindGroup::get_raw_ptr() const -> WGPUBindGroup
    {
        return m_handle->wgpu_bind_group;
    }

    auto BindGroup::get_raw_layout_ptr() const -> WGPUBindGroupLayout
    {
        return m_handle->wgpu_bind_group_layout;
    }

    BindGroup::Handle::~Handle()
    {
        if (wgpu_bind_group != nullptr)
        {
            wgpuBindGroupRelease(wgpu_bind_group);
            wgpu_bind_group = nullptr;
        }

        if (wgpu_bind_group_layout != nullptr)
        {
            wgpuBindGroupLayoutRelease(wgpu_bind_group_layout);
            wgpu_bind_group_layout = nullptr;
        }

        uniform_buffers = {};
        dynamic_size = INITIAL_DYNAMIC_SIZE;
    }

    auto BindGroup::Handle::rebuild() -> void
    {
        if (wgpu_bind_group != nullptr)
        {
            wgpuBindGroupRelease(wgpu_bind_group);
            wgpu_bind_group = nullptr;
        }

        if (wgpu_bind_group_layout != nullptr)
        {
            wgpuBindGroupLayoutRelease(wgpu_bind_group_layout);
            wgpu_bind_group_layout = nullptr;
        }

        vector<WGPUBindGroupLayoutEntry> layout_entries = wgpu_layout_entries;
        layout_entries.reserve(uniform_buffers.size());
        for (const auto &uniform_buffer: uniform_buffers)
        {
            layout_entries.push_back(uniform_buffer.get_wgpu_layout());
        }

        const auto layout_label = label + " Layout";
        const WGPUBindGroupLayoutDescriptor bind_group_layout_descriptor
        {
            .nextInChain = nullptr,
            .label = layout_label.c_str(),
            .entryCount = static_cast<u32>(layout_entries.size()),
            .entries = layout_entries.data(),
        };

        wgpu_bind_group_layout = wgpuDeviceCreateBindGroupLayout(device->get_raw_ptr(), &bind_group_layout_descriptor);

        vector<WGPUBindGroupEntry> entries = wgpu_entries;
        entries.reserve(uniform_buffers.size());
        for (const auto &uniform_buffer: uniform_buffers)
        {
            entries.push_back(uniform_buffer.get_wgpu_entry());
        }

        const WGPUBindGroupDescriptor bind_group_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
            .layout = wgpu_bind_group_layout,
            .entryCount = static_cast<u32>(entries.size()),
            .entries = entries.data(),
        };

        wgpu_bind_group = wgpuDeviceCreateBindGroup(device->get_raw_ptr(), &bind_group_descriptor);
    }

    auto BindGroup::Handle::grow_dynamic_size(u32 new_dynamic_size) -> void
    {
        MH_INFO("Growing dynamic size from {} to {} for bind group {}.", dynamic_size, new_dynamic_size, label);

        const CommandEncoder command_encoder{*device, label + " Command Encoder"};

        for (auto &uniform_buffer: uniform_buffers)
        {
            UniformBuffer old_buffer = uniform_buffer;

            uniform_buffer = UniformBuffer{
                *device, label, uniform_buffer.get_binding_idx(), new_dynamic_size,
                uniform_buffer.get_size_bytes()
            };

            command_encoder.copy_buffer_to_buffer(old_buffer, uniform_buffer, old_buffer.get_size_bytes());
        }

        const CommandBuffer command_buffer{command_encoder, label + " Command Buffer"};
        const Queue queue{*device};
        queue.submit(command_buffer);

        dynamic_size = new_dynamic_size;

        rebuild();
    }
}
