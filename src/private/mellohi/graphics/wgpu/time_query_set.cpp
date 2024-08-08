#include "mellohi/graphics/wgpu/time_query_set.hpp"

#include "mellohi/core/log.hpp"

namespace mellohi::wgpu
{
    TimeQuerySet::TimeQuerySet(const Device &device)
    {
        m_handle = std::make_shared<Handle>();

        WGPUBufferDescriptor buffer_descriptor
        {
            .nextInChain = nullptr,
            .label = "Mellohi Timer Resolve Buffer",
            .usage = WGPUBufferUsage_QueryResolve | WGPUBufferUsage_CopySrc,
            .size = 2 * sizeof(u64),
            .mappedAtCreation = false,
        };
        m_handle->wgpu_resolve_buffer = wgpuDeviceCreateBuffer(device.get_raw_ptr(), &buffer_descriptor);

        buffer_descriptor.label = "Mellohi Timer Map Buffer";
        buffer_descriptor.usage = WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst;
        m_handle->wgpu_map_buffer = wgpuDeviceCreateBuffer(device.get_raw_ptr(), &buffer_descriptor);

        constexpr WGPUQuerySetDescriptor query_set_descriptor
        {
            .nextInChain = nullptr,
            .label = "Timer Query Set",
            .type = WGPUQueryType_Timestamp,
            .count = 2,
        };
        m_handle->wgpu_query_set = wgpuDeviceCreateQuerySet(device.get_raw_ptr(), &query_set_descriptor);
    }

    auto TimeQuerySet::resolve(const CommandEncoder &command_encoder) const -> void
    {
        if (m_handle->mapping_ongoing)
        {
            return;
        }

        wgpuCommandEncoderResolveQuerySet(command_encoder.get_raw_ptr(), m_handle->wgpu_query_set, 0, 2,
                                          m_handle->wgpu_resolve_buffer, 0);
        wgpuCommandEncoderCopyBufferToBuffer(command_encoder.get_raw_ptr(), m_handle->wgpu_resolve_buffer, 0,
                                             m_handle->wgpu_map_buffer, 0, 2 * sizeof(u64));
    }

    auto TimeQuerySet::read_from_gpu() const -> void
    {
        if (m_handle->mapping_ongoing)
        {
            return;
        }

        m_handle->mapping_ongoing = true;
        wgpuBufferMapAsync(m_handle->wgpu_map_buffer, WGPUMapMode_Read, 0, 2 * sizeof(u64),
                           [](const WGPUBufferMapAsyncStatus status, void *user_data_ptr)
                           {
                               static_cast<Handle *>(user_data_ptr)->on_buffer_mapped(status);
                           }, m_handle.get());
    }

    auto TimeQuerySet::get_elapsed_ms() const -> f32
    {
        return m_handle->elapsed_ms;
    }

    auto TimeQuerySet::get_raw_ptr() const -> WGPUQuerySet
    {
        return m_handle->wgpu_query_set;
    }

    auto TimeQuerySet::Handle::on_buffer_mapped(const WGPUBufferMapAsyncStatus status) -> void
    {
        MH_ASSERT(status == WGPUBufferMapAsyncStatus_Success, "Failed to map timer buffer.");

        mapping_ongoing = false;

        const u64 *timestamp_data = static_cast<const u64 *>(wgpuBufferGetConstMappedRange(
            wgpu_map_buffer, 0, 2 * sizeof(u64)));

        const u64 begin = timestamp_data[0];
        const u64 end = timestamp_data[1];
        const u64 elapsed_ns = end - begin;
        elapsed_ms = static_cast<f32>(elapsed_ns) / 1e6;

        wgpuBufferUnmap(wgpu_map_buffer);
    }

    TimeQuerySet::Handle::~Handle()
    {
        if (wgpu_resolve_buffer != nullptr)
        {
            wgpuBufferRelease(wgpu_resolve_buffer);
        }

        if (wgpu_map_buffer != nullptr)
        {
            wgpuBufferRelease(wgpu_map_buffer);
        }

        if (wgpu_query_set != nullptr)
        {
            wgpuQuerySetRelease(wgpu_query_set);
        }
    }
}
