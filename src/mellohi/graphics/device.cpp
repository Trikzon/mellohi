#include "mellohi/graphics/device.hpp"

#include <glfw3webgpu.h>

namespace mellohi
{
    Surface::Surface(wgpu::Instance wgpu_instance, GLFWwindow *glfw_window)
    {
        m_wgpu_surface = glfwGetWGPUSurface(wgpu_instance, glfw_window);
    }

    Surface::~Surface()
    {
        if (m_wgpu_surface != nullptr)
        {
            m_wgpu_surface.unconfigure();
            m_wgpu_surface.release();
        }
    }

    Surface::Surface(Surface &&other) noexcept
    {
        std::swap(m_wgpu_surface, other.m_wgpu_surface);
    }

    Surface & Surface::operator=(Surface &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_surface, other.m_wgpu_surface);
        }

        return *this;
    }

    void Surface::configure(const u32 width, const u32 height, const Device &device, const bool vsync)
    {
        wgpu::SurfaceConfiguration configuration = {};
        configuration.width = width;
        configuration.height = height;
        configuration.usage = wgpu::TextureUsage::RenderAttachment;
        configuration.format = m_wgpu_texture_format;
        configuration.viewFormatCount = 0;
        configuration.viewFormats = nullptr;
        configuration.device = device.get_unsafe();
        configuration.presentMode = vsync ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Immediate;
        configuration.alphaMode = wgpu::CompositeAlphaMode::Auto;

        m_wgpu_surface.configure(configuration);
    }

    wgpu::TextureFormat Surface::get_wgpu_texture_format() const
    {
        return m_wgpu_texture_format;
    }

    std::optional<wgpu::TextureView> Surface::get_current_wgpu_texture_unsafe()
    {
        wgpu::SurfaceTexture surface_texture;
        m_wgpu_surface.getCurrentTexture(&surface_texture);
        if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::Success)
        {
            return std::nullopt;
        }

        wgpu::Texture texture = surface_texture.texture;

        wgpu::TextureViewDescriptor descriptor;
        descriptor.label = "Surface Texture View";
        descriptor.format = texture.getFormat();
        descriptor.dimension = wgpu::TextureViewDimension::_2D;
        descriptor.baseMipLevel = 0;
        descriptor.mipLevelCount = 1;
        descriptor.baseArrayLayer = 0;
        descriptor.arrayLayerCount = 1;
        descriptor.aspect = wgpu::TextureAspect::All;

        wgpu::TextureView texture_view = texture.createView(descriptor);
        texture.release();

        return texture_view;
    }

    void Surface::present()
    {
        m_wgpu_surface.present();
    }

    wgpu::Surface Surface::get_unsafe() const
    {
        return m_wgpu_surface;
    }

    Adapter::Adapter(wgpu::Instance wgpu_instance, const Surface &surface)
    {
        wgpu::RequestAdapterOptions request_adapter_options = {};
        request_adapter_options.compatibleSurface = surface.get_unsafe();

        m_wgpu_adapter = wgpu_instance.requestAdapter(request_adapter_options);

        if (m_wgpu_adapter == nullptr)
        {
            std::cerr << "Failed to request WebGPU adapter." << std::endl;
        }
    }

    Adapter::~Adapter()
    {
        if (m_wgpu_adapter != nullptr)
        {
            m_wgpu_adapter.release();
        }
    }

    Adapter::Adapter(Adapter &&other) noexcept
    {
        std::swap(m_wgpu_adapter, other.m_wgpu_adapter);
    }

    Adapter & Adapter::operator=(Adapter &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_adapter, other.m_wgpu_adapter);
        }

        return *this;
    }

    wgpu::Limits Adapter::get_limits()
    {
        wgpu::SupportedLimits supported_limits;
        m_wgpu_adapter.getLimits(&supported_limits);

        return supported_limits.limits;
    }

    wgpu::AdapterProperties Adapter::get_wgpu_properties()
    {
        wgpu::AdapterProperties properties;
        m_wgpu_adapter.getProperties(&properties);

        return properties;
    }

    wgpu::Adapter Adapter::get_unsafe() const
    {
        return m_wgpu_adapter;
    }

    Device::Device(Adapter &adapter)
    {
        m_hardware_limits = adapter.get_limits();
        m_wgpu_properties = adapter.get_wgpu_properties();

        wgpu::RequiredLimits required_limits = wgpu::Default;
        required_limits.limits.maxVertexAttributes = 3;
        required_limits.limits.maxVertexBuffers = 1;
        required_limits.limits.maxBufferSize = 1000000;
        required_limits.limits.maxVertexBufferArrayStride = 9 * sizeof(f32);
        required_limits.limits.minStorageBufferOffsetAlignment = m_hardware_limits.minStorageBufferOffsetAlignment;
        required_limits.limits.maxInterStageShaderComponents = 6;
        required_limits.limits.maxBindGroups = 2;
        required_limits.limits.maxUniformBuffersPerShaderStage = 1;
        required_limits.limits.maxUniformBufferBindingSize = 16 * 4;
        required_limits.limits.maxDynamicUniformBuffersPerPipelineLayout = 1;
        required_limits.limits.maxTextureDimension1D = 480;
        required_limits.limits.maxTextureDimension2D = 640;
        required_limits.limits.maxTextureArrayLayers = 1;

        wgpu::DeviceDescriptor descriptor = {};
        descriptor.label = "Mellohi Driver";
        descriptor.requiredFeatureCount = 0;
        descriptor.requiredLimits = &required_limits;
        descriptor.defaultQueue.nextInChain = nullptr;
        descriptor.defaultQueue.label = "Default Mellohi Queue";

        m_wgpu_device = adapter.get_unsafe().requestDevice(descriptor);

        wgpu::SupportedLimits supported_limits;
        m_wgpu_device.getLimits(&supported_limits);
        m_logical_limits = supported_limits.limits;

        m_error_callback_handle = m_wgpu_device.setUncapturedErrorCallback([](
            const wgpu::ErrorType type, char const *message)
            {
                std::cout << "Device error: type " << type;
                if (message) std::cout << " (message: " << message << ")";
                std::cout << std::endl;
            });
    }

    Device::~Device()
    {
        if (m_wgpu_device != nullptr)
        {
            m_wgpu_device.destroy();
            m_wgpu_device.release();
        }
    }

    Device::Device(Device &&other) noexcept
    {
        std::swap(m_wgpu_device, other.m_wgpu_device);
        std::swap(m_hardware_limits, other.m_hardware_limits);
        std::swap(m_logical_limits, other.m_logical_limits);
        std::swap(m_error_callback_handle, other.m_error_callback_handle);
    }

    Device & Device::operator=(Device &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_device, other.m_wgpu_device);
            std::swap(m_hardware_limits, other.m_hardware_limits);
            std::swap(m_logical_limits, other.m_logical_limits);
            std::swap(m_error_callback_handle, other.m_error_callback_handle);
        }

        return *this;
    }

    const wgpu::Limits & Device::get_hardware_limits() const
    {
        return m_hardware_limits;
    }

    const wgpu::Limits & Device::get_logical_limits() const
    {
        return m_logical_limits;
    }

    const wgpu::AdapterProperties & Device::get_wgpu_properties() const
    {
        return m_wgpu_properties;
    }

    void Device::tick()
    {
        m_wgpu_device.tick();
    }

    wgpu::Queue Device::get_queue_unsafe()
    {
        return m_wgpu_device.getQueue();
    }

    wgpu::Buffer Device::create_buffer_unsafe(const wgpu::BufferDescriptor &descriptor)
    {
        return m_wgpu_device.createBuffer(descriptor);
    }

    wgpu::ShaderModule Device::create_shader_module_unsafe(const wgpu::ShaderModuleDescriptor &descriptor)
    {
        return m_wgpu_device.createShaderModule(descriptor);
    }

    wgpu::RenderPipeline Device::create_render_pipeline_unsafe(const wgpu::RenderPipelineDescriptor &descriptor)
    {
        return m_wgpu_device.createRenderPipeline(descriptor);
    }

    wgpu::CommandEncoder Device::create_command_encoder_unsafe(const wgpu::CommandEncoderDescriptor &descriptor)
    {
        return m_wgpu_device.createCommandEncoder(descriptor);
    }

    wgpu::BindGroupLayout Device::create_bind_group_layout_unsafe(const wgpu::BindGroupLayoutDescriptor &descriptor)
    {
        return m_wgpu_device.createBindGroupLayout(descriptor);
    }

    wgpu::PipelineLayout Device::create_pipeline_layout_unsafe(const wgpu::PipelineLayoutDescriptor &descriptor)
    {
        return m_wgpu_device.createPipelineLayout(descriptor);
    }

    wgpu::BindGroup Device::create_bind_group_unsafe(const wgpu::BindGroupDescriptor &descriptor)
    {
        return m_wgpu_device.createBindGroup(descriptor);
    }

    wgpu::Texture Device::create_texture_unsafe(const wgpu::TextureDescriptor &descriptor)
    {
        return m_wgpu_device.createTexture(descriptor);
    }

    wgpu::Device Device::get_unsafe() const
    {
        return m_wgpu_device;
    }
}
