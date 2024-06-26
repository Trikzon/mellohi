#pragma once

#include <webgpu/webgpu.hpp>

struct GLFWwindow;

namespace mellohi
{
    struct DeviceLimits
    {
        uint32_t max_texture_dimension_1d;
        uint32_t max_texture_dimension_2d;
        uint32_t max_texture_dimension_3d;
        uint32_t max_texture_array_layers;
        uint32_t max_bind_groups;
        uint32_t max_bind_groups_plus_vertex_buffers;
        uint32_t max_bindings_per_bind_group;
        uint32_t max_dynamic_uniform_buffers_per_pipeline_layout;
        uint32_t max_dynamic_storage_buffers_per_pipeline_layout;
        uint32_t max_sampled_textures_per_shader_stage;
        uint32_t max_samplers_per_shader_stage;
        uint32_t max_storage_buffers_per_shader_stage;
        uint32_t max_storage_textures_per_shader_stage;
        uint32_t max_uniform_buffers_per_shader_stage;
        uint64_t max_uniform_buffer_binding_size;
        uint64_t max_storage_buffer_binding_size;
        uint32_t min_uniform_buffer_offset_alignment;
        uint32_t min_storage_buffer_offset_alignment;
        uint32_t max_vertex_buffers;
        uint64_t max_buffer_size;
        uint32_t max_vertex_attributes;
        uint32_t max_vertex_buffer_array_stride;
        uint32_t max_interstage_shader_components;
        uint32_t max_interstage_shader_variables;
        uint32_t max_color_attachments;
        uint32_t max_color_atteachment_bytes_per_sample;
        uint32_t max_compute_workgroup_storage_size;
        uint32_t max_compute_invocations_per_workgroup;
        uint32_t max_compute_workgroup_size_x;
        uint32_t max_compute_workgroup_size_y;
        uint32_t max_compute_workgroup_size_z;
        uint32_t max_compute_workgroups_per_dimension;
    };

    class Device;

    class Surface
    {
    public:
        Surface(wgpu::Instance wgpu_instance, GLFWwindow *glfw_window);
        ~Surface();

        Surface(const Surface &other) = delete;
        Surface(Surface &&other) noexcept;
        Surface & operator=(const Surface &other) = delete;
        Surface & operator=(Surface &&other) noexcept;

        void configure(uint32_t width, uint32_t height, const Device &device, bool vsync);
        wgpu::TextureFormat get_wgpu_texture_format() const;
        std::optional<wgpu::TextureView> get_current_wgpu_texture_unsafe();
        void present();

        wgpu::Surface get_unsafe() const;

    private:
        wgpu::Surface m_wgpu_surface;
        // With Dawn WebGPU backend, the preferred format is always BGRA8Unorm.
        wgpu::TextureFormat m_wgpu_texture_format = wgpu::TextureFormat::BGRA8Unorm;
    };

    class Adapter
    {
    public:
        Adapter(wgpu::Instance wgpu_instance, const Surface &surface);
        ~Adapter();

        Adapter(const Adapter &other) = delete;
        Adapter(Adapter &&other) noexcept;
        Adapter & operator=(const Adapter &other) = delete;
        Adapter & operator=(Adapter &&other) noexcept;

        DeviceLimits get_limits();
        wgpu::AdapterProperties get_wgpu_properties();

        wgpu::Adapter get_unsafe() const;

    private:
        wgpu::Adapter m_wgpu_adapter;
    };

    class Device
    {
    public:
        explicit Device(Adapter &adapter);
        ~Device();

        Device(const Device &other) = delete;
        Device(Device &&other) noexcept;
        Device & operator=(const Device &other) = delete;
        Device & operator=(Device &&other) noexcept;

        const DeviceLimits & get_hardware_limits() const;
        const DeviceLimits & get_logical_limits() const;
        const wgpu::AdapterProperties & get_wgpu_properties() const;

        void tick();

        wgpu::Queue get_queue_unsafe();
        wgpu::Buffer create_buffer_unsafe(const wgpu::BufferDescriptor &descriptor);
        wgpu::ShaderModule create_shader_module_unsafe(const wgpu::ShaderModuleDescriptor &descriptor);
        wgpu::RenderPipeline create_render_pipeline_unsafe(const wgpu::RenderPipelineDescriptor &descriptor);
        wgpu::CommandEncoder create_command_encoder_unsafe(const wgpu::CommandEncoderDescriptor &descriptor);
        wgpu::BindGroupLayout create_bind_group_layout_unsafe(const wgpu::BindGroupLayoutDescriptor &descriptor);
        wgpu::PipelineLayout create_pipeline_layout_unsafe(const wgpu::PipelineLayoutDescriptor &descriptor);
        wgpu::BindGroup create_bind_group_unsafe(const wgpu::BindGroupDescriptor &descriptor);
        wgpu::Texture create_texture_unsafe(const wgpu::TextureDescriptor &descriptor);

        wgpu::Device get_unsafe() const;

    private:
        wgpu::Device m_wgpu_device;
        DeviceLimits m_hardware_limits;
        DeviceLimits m_logical_limits;
        wgpu::AdapterProperties m_wgpu_properties;

        std::unique_ptr<wgpu::ErrorCallback> m_error_callback_handle;
    };
}
