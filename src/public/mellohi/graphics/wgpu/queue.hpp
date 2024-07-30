#pragma once

#include "mellohi/graphics/wgpu/command_buffer.hpp"
#include "mellohi/graphics/wgpu/index_buffer.hpp"
#include "mellohi/graphics/wgpu/uniform_buffer.hpp"
#include "mellohi/graphics/wgpu/vertex_buffer.hpp"

namespace mellohi::wgpu
{
    class Queue
    {
    public:
        explicit Queue(const Device &device);
        ~Queue();
        Queue(const Queue &other);
        Queue(Queue &&other) noexcept;
        auto operator=(const Queue &other) -> Queue &;
        auto operator=(Queue &&other) noexcept -> Queue &;

        auto submit(const CommandBuffer &command_buffer) const -> void;
        auto write_buffer(const IndexBuffer &index_buffer, const void *data, usize size_bytes) const -> void;
        auto write_buffer(const UniformBuffer &uniform_buffer, u32 dynamic_idx, const void *data,
                          usize size_bytes) const -> void;
        auto write_buffer(const VertexBuffer &vertex_buffer, const void *data, usize size_bytes) const -> void;

        auto get_raw_ptr() const -> WGPUQueue;

    private:
        WGPUQueue m_wgpu_queue{nullptr};
    };
}
