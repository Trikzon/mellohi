#include "mellohi/graphics/bind_group.h"

namespace mellohi
{
    BindGroup::BindGroup(Device &device, const uint32_t initial_dynamic_size) : m_dynamic_size(initial_dynamic_size)
    {
        // TODO: Assert that initial_dynamic_count is 0
        rebuild_bind_group(device);
    }

    BindGroup::~BindGroup()
    {
        if (m_wgpu_bind_group != nullptr)
        {
            m_wgpu_bind_group.release();
        }

        if (m_wgpu_bind_group_layout != nullptr)
        {
            m_wgpu_bind_group_layout.release();
        }
    }

    BindGroup::BindGroup(BindGroup &&other) noexcept : m_dynamic_size(other.m_dynamic_size)
    {
        std::swap(m_wgpu_bind_group, other.m_wgpu_bind_group);
        std::swap(m_wgpu_bind_group_layout, other.m_wgpu_bind_group_layout);
        std::swap(m_uniform_buffers, other.m_uniform_buffers);
    }

    BindGroup & BindGroup::operator=(BindGroup &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_bind_group, other.m_wgpu_bind_group);
            std::swap(m_wgpu_bind_group_layout, other.m_wgpu_bind_group_layout);
            std::swap(m_uniform_buffers, other.m_uniform_buffers);
            m_dynamic_size = other.m_dynamic_size;
        }

        return *this;
    }

    void BindGroup::add(Device &device, uint32_t binding_idx, uint32_t size_bytes)
    {
        m_uniform_buffers.emplace_back(device, binding_idx, m_dynamic_size, size_bytes);

        rebuild_bind_group(device);
    }

    void BindGroup::write(Device &device, const uint32_t binding_idx, const uint32_t dynamic_idx, const void *data)
    {
        if (dynamic_idx >= m_dynamic_size)
        {
            grow_dynamic_size(device, static_cast<uint32_t>(m_dynamic_size * 1.5 + 1));
        }

        for (UniformBuffer &uniform_buffer : m_uniform_buffers)
        {
            if (uniform_buffer.get_binding_idx() == binding_idx)
            {
                uniform_buffer.write(device, dynamic_idx, data);
            }
            // TODO: Assert that uniform_buffer is not nullptr
        }
    }

    std::vector<uint32_t> BindGroup::get_dynamic_offsets(Device &device, const uint32_t dynamic_idx)
    {
        if (dynamic_idx >= m_dynamic_size)
        {
            grow_dynamic_size(device, static_cast<uint32_t>(m_dynamic_size * 1.5 + 1));
        }

        std::vector<uint32_t> dynamic_offsets;

        dynamic_offsets.reserve(m_uniform_buffers.size());
        for (const UniformBuffer &uniform_buffer : m_uniform_buffers)
        {
            dynamic_offsets.push_back(dynamic_idx * uniform_buffer.get_stride_bytes());
        }

        return dynamic_offsets;
    }

    wgpu::BindGroup BindGroup::get_unsafe() const
    {
        return m_wgpu_bind_group;
    }

    wgpu::BindGroupLayout BindGroup::get_layout_unsafe() const
    {
        return m_wgpu_bind_group_layout;
    }

    void BindGroup::rebuild_bind_group(Device &device)
    {
        if (m_wgpu_bind_group != nullptr)
        {
            m_wgpu_bind_group.release();
        }

        if (m_wgpu_bind_group_layout != nullptr)
        {
            m_wgpu_bind_group_layout.release();
        }

        std::vector<wgpu::BindGroupLayoutEntry> uniform_layouts;
        uniform_layouts.reserve(m_uniform_buffers.size());
        for (const UniformBuffer &uniform_buffer : m_uniform_buffers)
        {
            uniform_layouts.push_back(uniform_buffer.get_wgpu_layout());
        }
        wgpu::BindGroupLayoutDescriptor bind_group_layout_descriptor;
        bind_group_layout_descriptor.entryCount = uniform_layouts.size();
        bind_group_layout_descriptor.entries = uniform_layouts.data();
        m_wgpu_bind_group_layout = device.create_bind_group_layout_unsafe(bind_group_layout_descriptor);

        std::vector<wgpu::BindGroupEntry> uniform_entries;
        uniform_entries.reserve(m_uniform_buffers.size());
        for (const UniformBuffer &uniform_buffer : m_uniform_buffers)
        {
            uniform_entries.push_back(uniform_buffer.get_wgpu_entry());
        }
        wgpu::BindGroupDescriptor bind_group_descriptor;
        bind_group_descriptor.layout = m_wgpu_bind_group_layout;
        bind_group_descriptor.entryCount = uniform_entries.size();
        bind_group_descriptor.entries = uniform_entries.data();
        m_wgpu_bind_group = device.create_bind_group_unsafe(bind_group_descriptor);
    }

    void BindGroup::grow_dynamic_size(Device &device, const uint32_t new_dynamic_size)
    {
        std::cout << "INFO: Growing dynamic size from " << m_dynamic_size << " to " << new_dynamic_size << std::endl;

        wgpu::CommandEncoderDescriptor command_encoder_descriptor = {};
        command_encoder_descriptor.label = "Bind Group Command Encoder";
        wgpu::CommandEncoder command_encoder = device.create_command_encoder_unsafe(command_encoder_descriptor);

        for (auto & m_uniform_buffer : m_uniform_buffers)
        {
            wgpu::Buffer old_buffer = m_uniform_buffer.get_unsafe();
            old_buffer.addRef();

            m_uniform_buffer = UniformBuffer(device, m_uniform_buffer.get_binding_idx(), new_dynamic_size, m_uniform_buffer.get_size_bytes());

            command_encoder.copyBufferToBuffer(old_buffer, 0, m_uniform_buffer.get_unsafe(), 0, old_buffer.getSize());
            old_buffer.release();
        }

        wgpu::CommandBuffer command_buffer = command_encoder.finish();
        command_encoder.release();
        wgpu::Queue queue = device.get_queue_unsafe();
        queue.submit(1, &command_buffer);
        command_buffer.release();
        queue.release();

        m_dynamic_size = new_dynamic_size;

        rebuild_bind_group(device);
    }
}
