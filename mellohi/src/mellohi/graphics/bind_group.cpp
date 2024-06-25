#include "mellohi/graphics/bind_group.h"

#include "mellohi/game.h"

namespace mellohi
{
    BindGroup::BindGroup(Pipeline &pipeline, const UniformBuffer &uniform_buffer)
    {
        Device &device = Game::get().get_window().get_device();

        auto bind_group_layout = pipeline.get_wgpu_bind_group_layout_unsafe(0);
        const auto bind_group_entry = uniform_buffer.get_wgpu_bind_group_entry();

        wgpu::BindGroupDescriptor bind_group_descriptor;
        bind_group_descriptor.layout = bind_group_layout;
        bind_group_descriptor.entryCount = 1;
        bind_group_descriptor.entries = &bind_group_entry;
        m_wgpu_bind_group = device.create_bind_group_unsafe(bind_group_descriptor);

        bind_group_layout.release();
    }

    BindGroup::~BindGroup()
    {
        if (m_wgpu_bind_group != nullptr)
        {
            m_wgpu_bind_group.release();
        }
    }

    wgpu::BindGroup BindGroup::get_unsafe() const
    {
        return m_wgpu_bind_group;
    }
}
