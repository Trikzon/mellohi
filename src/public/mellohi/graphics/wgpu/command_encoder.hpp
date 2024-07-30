#pragma once

#include "mellohi/graphics/wgpu/uniform_buffer.hpp"

namespace mellohi::wgpu
{
    class CommandEncoder
    {
    public:
        CommandEncoder(const Device &device, const string &label);
        ~CommandEncoder();
        CommandEncoder(const CommandEncoder &other);
        CommandEncoder(CommandEncoder &&other) noexcept;
        auto operator=(const CommandEncoder &other) -> CommandEncoder &;
        auto operator=(CommandEncoder &&other) noexcept -> CommandEncoder &;

        auto copy_buffer_to_buffer(const UniformBuffer &source, const UniformBuffer &destination,
                                   const u32 size_bytes) const -> void;

        auto get_raw_ptr() const -> WGPUCommandEncoder;

    private:
        WGPUCommandEncoder m_wgpu_command_encoder{nullptr};
    };
}
