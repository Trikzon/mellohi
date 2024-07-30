#pragma once

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    class IndexBuffer
    {
    public:
        IndexBuffer(const Device &device, const string &label, const vector<u16> &data);
        IndexBuffer(const Device &device, const string &label, const vector<u32> &data);
        ~IndexBuffer();
        IndexBuffer(const IndexBuffer &other);
        IndexBuffer(IndexBuffer &&other) noexcept;
        auto operator=(const IndexBuffer &other) -> IndexBuffer &;
        auto operator=(IndexBuffer &&other) noexcept -> IndexBuffer &;

        auto get_size_bytes() const -> u64;
        auto get_index_count() const -> u32;

        auto get_wgpu_index_format() const -> WGPUIndexFormat;

        auto get_raw_ptr() const -> WGPUBuffer;

    private:
        WGPUBuffer m_wgpu_buffer{nullptr};
        u64 m_size_bytes{0};
        WGPUIndexFormat m_wgpu_index_format{WGPUIndexFormat_Undefined};
        u32 m_index_count{0};

        auto create(const Device &device, const string &label, const void *data) -> void;
    };
}
