#include "mellohi/graphics/pipeline.hpp"

namespace mellohi
{
    Pipeline::Pipeline(Device &device, Surface &surface, const AssetId &shader_asset_id, VertexBuffer &vertex_buffer,
        const BindGroup *bind_groups, usize bind_group_count)
    {
        std::string shader_code = shader_asset_id.read_file_to_string();

        wgpu::ShaderModuleWGSLDescriptor shader_module_wgsl_descriptor;
        shader_module_wgsl_descriptor.chain.next = nullptr;
        shader_module_wgsl_descriptor.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
        shader_module_wgsl_descriptor.code = shader_code.c_str();

        wgpu::ShaderModuleDescriptor shader_module_descriptor;
        shader_module_descriptor.nextInChain = &shader_module_wgsl_descriptor.chain;

        {
            wgpu::ShaderModule shader_module = device.create_shader_module_unsafe(shader_module_descriptor);

            const auto vertex_buffer_layout = vertex_buffer.get_wgpu_layout();
            wgpu::RenderPipelineDescriptor descriptor;
            descriptor.label = "Mellohi Pipeline";

            descriptor.vertex.bufferCount = 1;
            descriptor.vertex.buffers = &vertex_buffer_layout;
            descriptor.vertex.module = shader_module;
            descriptor.vertex.entryPoint = "vertex_main";
            descriptor.vertex.constantCount = 0;
            descriptor.vertex.constants = nullptr;

            descriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
            descriptor.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
            descriptor.primitive.frontFace = wgpu::FrontFace::CCW;
            descriptor.primitive.cullMode = wgpu::CullMode::None;

            wgpu::FragmentState fragment_state;
            fragment_state.module = shader_module;
            fragment_state.entryPoint = "fragment_main";
            fragment_state.constantCount = 0;
            fragment_state.constants = nullptr;

            wgpu::BlendState blend_state;
            blend_state.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
            blend_state.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
            blend_state.color.operation = wgpu::BlendOperation::Add;
            blend_state.alpha.srcFactor = wgpu::BlendFactor::Zero;
            blend_state.alpha.dstFactor = wgpu::BlendFactor::One;
            blend_state.alpha.operation = wgpu::BlendOperation::Add;

            wgpu::ColorTargetState color_target;
            color_target.format = surface.get_wgpu_texture_format();
            color_target.blend = &blend_state;
            color_target.writeMask = wgpu::ColorWriteMask::All;

            fragment_state.targetCount = 1;
            fragment_state.targets = &color_target;

            descriptor.fragment = &fragment_state;

            wgpu::DepthStencilState depth_stencil_state = wgpu::Default;
            depth_stencil_state.depthCompare = wgpu::CompareFunction::Less;
            depth_stencil_state.depthWriteEnabled = true;
            wgpu::TextureFormat depth_texture_format = wgpu::TextureFormat::Depth24Plus;
            depth_stencil_state.format = depth_texture_format;
            depth_stencil_state.stencilReadMask = 0;
            depth_stencil_state.stencilWriteMask = 0;
            descriptor.depthStencil = &depth_stencil_state;

            descriptor.multisample.count = 1;
            descriptor.multisample.mask = ~0u;
            descriptor.multisample.alphaToCoverageEnabled = false;

            std::vector<wgpu::BindGroupLayout> bind_group_layouts;
            bind_group_layouts.reserve(bind_group_count);
            for (auto i = 0; i < bind_group_count; ++i)
            {
                bind_group_layouts.push_back(bind_groups[i].get_layout_unsafe());
            }

            wgpu::PipelineLayoutDescriptor layout_descriptor;
            layout_descriptor.bindGroupLayoutCount = 1;
            layout_descriptor.bindGroupLayouts = reinterpret_cast<WGPUBindGroupLayout *>(bind_group_layouts.data());
            wgpu::PipelineLayout layout = device.create_pipeline_layout_unsafe(layout_descriptor);

            descriptor.layout = layout;

            m_wgpu_render_pipeline = device.create_render_pipeline_unsafe(descriptor);

            shader_module.release();
            layout.release();
        }
    }

    Pipeline::~Pipeline()
    {
        if (m_wgpu_render_pipeline != nullptr)
        {
            m_wgpu_render_pipeline.release();
        }
    }

    Pipeline::Pipeline(Pipeline &&other) noexcept
    {
        std::swap(m_wgpu_render_pipeline, other.m_wgpu_render_pipeline);
    }

    Pipeline & Pipeline::operator=(Pipeline &&other) noexcept
    {
        if (this != &other)
        {
            std::swap(m_wgpu_render_pipeline,other.m_wgpu_render_pipeline);
        }

        return *this;
    }

    wgpu::RenderPipeline Pipeline::get_unsafe() const
    {
        return m_wgpu_render_pipeline;
    }
}
