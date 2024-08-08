#pragma once

#include "mellohi/graphics/wgpu/command_encoder.hpp"

namespace mellohi::wgpu
{
    class TimeQuerySet
    {
    public:
        TimeQuerySet(const Device &device);

        auto resolve(const CommandEncoder &command_encoder) const -> void;
        auto read_from_gpu() const -> void;

        auto get_elapsed_ms() const -> f32;

        auto get_raw_ptr() const -> WGPUQuerySet;

    private:
        struct Handle
        {
            f32 elapsed_ms{0.0f};

            bool mapping_ongoing{false};
            WGPUBuffer wgpu_resolve_buffer{nullptr};
            WGPUBuffer wgpu_map_buffer{nullptr};
            WGPUQuerySet wgpu_query_set{nullptr};

            ~Handle();

            auto on_buffer_mapped(WGPUBufferMapAsyncStatus status) -> void;
        };

        s_ptr<Handle> m_handle{nullptr};
    };
}
