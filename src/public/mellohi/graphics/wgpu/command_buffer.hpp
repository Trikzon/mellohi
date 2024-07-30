#pragma once

#include "mellohi/graphics/wgpu/command_encoder.hpp"

namespace mellohi::wgpu
{
    class CommandBuffer
    {
    public:
        CommandBuffer(const CommandEncoder &command_encoder, const string &label);
        ~CommandBuffer();
        CommandBuffer(const CommandBuffer &other);
        CommandBuffer(CommandBuffer &&other) noexcept;
        auto operator=(const CommandBuffer &other) -> CommandBuffer &;
        auto operator=(CommandBuffer &&other) noexcept -> CommandBuffer &;

        auto get_raw_ptr() const -> WGPUCommandBuffer;

    private:
        WGPUCommandBuffer m_wgpu_command_buffer{nullptr};
    };
}
