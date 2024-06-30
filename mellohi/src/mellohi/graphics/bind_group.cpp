#include "mellohi/graphics/bind_group.h"

namespace mellohi
{
    BindGroup::BindGroup(Device &device, const UniformBuffer &uniform_buffer)
    {
        const wgpu::BindGroupLayoutEntry uniform_layout = uniform_buffer.get_wgpu_layout();
        wgpu::BindGroupLayoutDescriptor bind_group_layout_descriptor;
        bind_group_layout_descriptor.entryCount = 1;
        bind_group_layout_descriptor.entries = &uniform_layout;
        m_wgpu_bind_group_layout = device.create_bind_group_layout_unsafe(bind_group_layout_descriptor);

        const wgpu::BindGroupEntry uniform_entry = uniform_buffer.get_wgpu_entry();
        wgpu::BindGroupDescriptor bind_group_descriptor;
        bind_group_descriptor.layout = m_wgpu_bind_group_layout;
        bind_group_descriptor.entryCount = 1;
        bind_group_descriptor.entries = &uniform_entry;
        m_wgpu_bind_group = device.create_bind_group_unsafe(bind_group_descriptor);
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

    BindGroup::BindGroup(const BindGroup &other)
    {
        m_wgpu_bind_group = other.m_wgpu_bind_group;
        m_wgpu_bind_group.addRef();
        m_wgpu_bind_group_layout = other.m_wgpu_bind_group_layout;
        m_wgpu_bind_group_layout.addRef();
    }

    BindGroup::BindGroup(BindGroup &&other) noexcept
    {
        std::swap(m_wgpu_bind_group, other.m_wgpu_bind_group);
        std::swap(m_wgpu_bind_group_layout, other.m_wgpu_bind_group_layout);
    }

    BindGroup & BindGroup::operator=(const BindGroup &other)
    {
        if (this != &other)
        {
            this->~BindGroup();

            m_wgpu_bind_group = other.m_wgpu_bind_group;
            m_wgpu_bind_group.addRef();
            m_wgpu_bind_group_layout = other.m_wgpu_bind_group_layout;
            m_wgpu_bind_group_layout.addRef();
        }

        return *this;
    }

    BindGroup & BindGroup::operator=(BindGroup &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_bind_group, other.m_wgpu_bind_group);
            std::swap(m_wgpu_bind_group_layout, other.m_wgpu_bind_group_layout);
        }

        return *this;
    }

    wgpu::BindGroup BindGroup::get_unsafe() const
    {
        return m_wgpu_bind_group;
    }

    wgpu::BindGroupLayout BindGroup::get_layout_unsafe() const
    {
        return m_wgpu_bind_group_layout;
    }
}
