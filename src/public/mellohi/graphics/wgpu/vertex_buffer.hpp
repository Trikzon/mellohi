#pragma once

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    class VertexBuffer
    {
    public:
        template<typename T>
        VertexBuffer(const Device &device, const string &label, const vector<T> &data);
        ~VertexBuffer();
        VertexBuffer(const VertexBuffer &other);
        VertexBuffer(VertexBuffer &&other) noexcept;
        auto operator=(const VertexBuffer &other) -> VertexBuffer &;
        auto operator=(VertexBuffer &&other) noexcept -> VertexBuffer &;

        template<typename T>
        auto add_attribute(u32 shader_location) const -> void;

        auto get_size_bytes() const -> u64;
        auto get_stride_bytes() const -> u64;

        auto get_wgpu_layout() const -> WGPUVertexBufferLayout;

        auto get_raw_ptr() const -> WGPUBuffer;

    private:
        WGPUBuffer m_wgpu_buffer{nullptr};
        u64 m_size_bytes{0};
        s_ptr<vector<WGPUVertexAttribute>> m_wgpu_vertex_attributes{new vector<WGPUVertexAttribute>{}};
        s_ptr<u64> m_stride_bytes{new u64{0}};

        auto create(const Device &device, const string &label, const void *data) -> void;
        auto add_attribute(u32 shader_location, WGPUVertexFormat format, u32 size_bytes) const -> void;
    };

    // ===== Template Definitions =====

    template<typename T>
    VertexBuffer::VertexBuffer(const Device &device, const string &label, const vector<T> &data)
    {
        m_size_bytes = data.size() * sizeof(T);
        MH_ASSERT(m_size_bytes % 4 == 0, "Vertex buffer size must be a multiple of 4 bytes.");

        create(device, label, data.data());
    }

    template<typename T>
    auto VertexBuffer::add_attribute(const u32 shader_location) const -> void
    {
        MH_ASSERT(false, "Unsupported vertex attribute type.");
    }

    template<>
    inline auto VertexBuffer::add_attribute<f32>(const u32 shader_location) const -> void
    {
        add_attribute(shader_location, WGPUVertexFormat_Float32, sizeof(f32));
    }

    template<>
    inline auto VertexBuffer::add_attribute<vec2f>(const u32 shader_location) const -> void
    {
        add_attribute(shader_location, WGPUVertexFormat_Float32x2, sizeof(vec2f));
    }

    template<>
    inline auto VertexBuffer::add_attribute<vec3f>(const u32 shader_location) const -> void
    {
        add_attribute(shader_location, WGPUVertexFormat_Float32x3, sizeof(vec3f));
    }

    template<>
    inline auto VertexBuffer::add_attribute<vec4f>(const u32 shader_location) const -> void
    {
        add_attribute(shader_location, WGPUVertexFormat_Float32x4, sizeof(vec4f));
    }
}
