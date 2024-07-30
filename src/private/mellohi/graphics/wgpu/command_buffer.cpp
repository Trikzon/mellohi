#include "mellohi/graphics/wgpu/command_buffer.hpp"

namespace mellohi::wgpu
{
    CommandBuffer::CommandBuffer(const CommandEncoder &command_encoder, const string &label)
    {
        const WGPUCommandBufferDescriptor command_buffer_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
        };

        m_wgpu_command_buffer = wgpuCommandEncoderFinish(command_encoder.get_raw_ptr(), &command_buffer_descriptor);
    }

    CommandBuffer::~CommandBuffer()
    {
        if (m_wgpu_command_buffer != nullptr)
        {
            wgpuCommandBufferRelease(m_wgpu_command_buffer);
            m_wgpu_command_buffer = nullptr;
        }
    }

    CommandBuffer::CommandBuffer(const CommandBuffer &other)
    {
        m_wgpu_command_buffer = other.m_wgpu_command_buffer;
        if (m_wgpu_command_buffer != nullptr)
        {
            wgpuCommandBufferAddRef(m_wgpu_command_buffer);
        }
    }

    CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
    {
        std::swap(m_wgpu_command_buffer, other.m_wgpu_command_buffer);
    }

    auto CommandBuffer::operator=(const CommandBuffer &other) -> CommandBuffer &
    {
        if (this != &other)
        {
            if (m_wgpu_command_buffer != nullptr)
            {
                wgpuCommandBufferRelease(m_wgpu_command_buffer);
            }

            m_wgpu_command_buffer = other.m_wgpu_command_buffer;
            if (m_wgpu_command_buffer != nullptr)
            {
                wgpuCommandBufferAddRef(m_wgpu_command_buffer);
            }
        }

        return *this;
    }

    auto CommandBuffer::operator=(CommandBuffer &&other) noexcept -> CommandBuffer &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_command_buffer, other.m_wgpu_command_buffer);
        }

        return *this;
    }

    auto CommandBuffer::get_raw_ptr() const -> WGPUCommandBuffer
    {
        return m_wgpu_command_buffer;
    }
}
