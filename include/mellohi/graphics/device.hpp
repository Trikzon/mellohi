#pragma once

#include <webgpu/webgpu.hpp>

#include "mellohi/types.hpp"

struct GLFWwindow;

namespace mellohi
{
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

        void configure(u32 width, u32 height, const Device &device, bool vsync);
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

        wgpu::Limits get_limits();
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

        const wgpu::Limits & get_hardware_limits() const;
        const wgpu::Limits & get_logical_limits() const;
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
        wgpu::Limits m_hardware_limits;
        wgpu::Limits m_logical_limits;
        wgpu::AdapterProperties m_wgpu_properties;

        std::unique_ptr<wgpu::ErrorCallback> m_error_callback_handle;
    };
}
