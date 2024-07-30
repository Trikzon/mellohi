#include "mellohi/graphics/wgpu/command_encoder.hpp"

namespace mellohi::wgpu
{
    CommandEncoder::CommandEncoder(const Device &device, const string &label)
    {
        WGPUCommandEncoderDescriptor command_encoder_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
        };

        m_wgpu_command_encoder = wgpuDeviceCreateCommandEncoder(device.get_raw_ptr(), &command_encoder_descriptor);
    }

    CommandEncoder::~CommandEncoder()
    {
        if (m_wgpu_command_encoder != nullptr)
        {
            wgpuCommandEncoderRelease(m_wgpu_command_encoder);
            m_wgpu_command_encoder = nullptr;
        }
    }

    CommandEncoder::CommandEncoder(const CommandEncoder &other)
    {
        m_wgpu_command_encoder = other.m_wgpu_command_encoder;
        if (m_wgpu_command_encoder != nullptr)
        {
            wgpuCommandEncoderAddRef(m_wgpu_command_encoder);
        }
    }

    CommandEncoder::CommandEncoder(CommandEncoder &&other) noexcept
    {
        std::swap(m_wgpu_command_encoder, other.m_wgpu_command_encoder);
    }

    auto CommandEncoder::operator=(const CommandEncoder &other) -> CommandEncoder &
    {
        if (this != &other)
        {
            if (m_wgpu_command_encoder != nullptr)
            {
                wgpuCommandEncoderRelease(m_wgpu_command_encoder);
            }

            m_wgpu_command_encoder = other.m_wgpu_command_encoder;
            if (m_wgpu_command_encoder != nullptr)
            {
                wgpuCommandEncoderAddRef(m_wgpu_command_encoder);
            }
        }

        return *this;
    }

    auto CommandEncoder::operator=(CommandEncoder &&other) noexcept -> CommandEncoder &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_command_encoder, other.m_wgpu_command_encoder);
        }

        return *this;
    }

    auto CommandEncoder::copy_buffer_to_buffer(const UniformBuffer &source, const UniformBuffer &destination,
                                               const u32 size_bytes) const -> void
    {
        MH_ASSERT(source.get_size_bytes() == destination.get_size_bytes() && source.get_size_bytes() == size_bytes,
                  "Source and destination buffer sizes must match.");

        wgpuCommandEncoderCopyBufferToBuffer(m_wgpu_command_encoder, source.get_raw_ptr(), 0, destination.get_raw_ptr(),
                                             0, size_bytes);
    }

    auto CommandEncoder::get_raw_ptr() const -> WGPUCommandEncoder
    {
        return m_wgpu_command_encoder;
    }
}
