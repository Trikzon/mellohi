#include "mellohi/graphics/wgpu/render_pipeline.hpp"

namespace mellohi::wgpu
{
    RenderPipeline::RenderPipeline(const Device &device, const string &label, const ShaderModule &shader_module,
                                   const vector<VertexBuffer> &vertex_buffers, const vector<BindGroup> &bind_groups)
    {
        vector<WGPUBindGroupLayout> bind_group_layouts;
        bind_group_layouts.reserve(bind_groups.size());

        for (const auto &bind_group: bind_groups)
        {
            bind_group_layouts.push_back(bind_group.get_raw_layout_ptr());
        }

        const auto layout_label = label + " Layout";
        WGPUPipelineLayoutDescriptor pipeline_layout_descriptor
        {
            .nextInChain = nullptr,
            .label = layout_label.c_str(),
            .bindGroupLayoutCount = static_cast<u32>(bind_group_layouts.size()),
            .bindGroupLayouts = bind_group_layouts.data(),
        };

        WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(device.get_raw_ptr(),
                                                                            &pipeline_layout_descriptor);

        // TODO: Move to a constant.
        constexpr WGPUTextureFormat depth_texture_format = WGPUTextureFormat_Depth24Plus;
        constexpr WGPUDepthStencilState depth_stencil_state
        {
            .nextInChain = nullptr,
            .format = depth_texture_format,
            .depthWriteEnabled = true,
            .depthCompare = WGPUCompareFunction_Less,
            // .stencilFront = ,
            // .stencilBack = ,
            .stencilReadMask = 0,
            .stencilWriteMask = 0,
            .depthBias = 0,
            .depthBiasSlopeScale = 0.0f,
            .depthBiasClamp = 0.0f,
        };

        constexpr WGPUBlendState blend_state
        {
            .color = WGPUBlendComponent
            {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_SrcAlpha,
                .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
            },
            .alpha = WGPUBlendComponent
            {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_Zero,
                .dstFactor = WGPUBlendFactor_One,
            }
        };

        const WGPUColorTargetState color_target_state
        {
            .nextInChain = nullptr,
            // TODO: Move to a constant.
            .format = WGPUTextureFormat_BGRA8Unorm,
            .blend = &blend_state,
            .writeMask = WGPUColorWriteMask_All,
        };

        const WGPUFragmentState fragment_state
        {
            .nextInChain = nullptr,
            .module = shader_module.get_raw_ptr(),
            .entryPoint = "fragment_main",
            .constantCount = 0,
            .constants = nullptr,
            .targetCount = 1,
            .targets = &color_target_state,
        };

        vector<WGPUVertexBufferLayout> vertex_buffer_layouts;
        vertex_buffer_layouts.reserve(vertex_buffers.size());
        for (const auto &vertex_buffer: vertex_buffers)
        {
            vertex_buffer_layouts.push_back(vertex_buffer.get_wgpu_layout());
        }

        const WGPURenderPipelineDescriptor render_pipeline_descriptor
        {
            .nextInChain = nullptr,
            .label = "Mellohi Render Pipeline",
            .layout = pipeline_layout,
            .vertex = WGPUVertexState
            {
                .nextInChain = nullptr,
                .module = shader_module.get_raw_ptr(),
                .entryPoint = "vertex_main",
                .constantCount = 0,
                .constants = nullptr,
                .bufferCount = vertex_buffer_layouts.size(),
                .buffers = vertex_buffer_layouts.data(),
            },
            .primitive = WGPUPrimitiveState
            {
                .nextInChain = nullptr,
                .topology = WGPUPrimitiveTopology_TriangleList,
                .stripIndexFormat = WGPUIndexFormat_Undefined,
                .frontFace = WGPUFrontFace_CCW,
                .cullMode = WGPUCullMode_Front,
            },
            .depthStencil = &depth_stencil_state,
            .multisample = WGPUMultisampleState
            {
                .nextInChain = nullptr,
                .count = 1,
                .mask = ~0u,
                .alphaToCoverageEnabled = false,
            },
            .fragment = &fragment_state,
        };

        m_wgpu_render_pipeline = wgpuDeviceCreateRenderPipeline(device.get_raw_ptr(), &render_pipeline_descriptor);

        wgpuPipelineLayoutRelease(pipeline_layout);
    }

    RenderPipeline::~RenderPipeline()
    {
        if (m_wgpu_render_pipeline != nullptr)
        {
            wgpuRenderPipelineRelease(m_wgpu_render_pipeline);
            m_wgpu_render_pipeline = nullptr;
        }
    }

    RenderPipeline::RenderPipeline(const RenderPipeline &other)
    {
        m_wgpu_render_pipeline = other.m_wgpu_render_pipeline;
        if (m_wgpu_render_pipeline != nullptr)
        {
            wgpuRenderPipelineAddRef(m_wgpu_render_pipeline);
        }
    }

    RenderPipeline::RenderPipeline(RenderPipeline &&other) noexcept
    {
        std::swap(m_wgpu_render_pipeline, other.m_wgpu_render_pipeline);
    }

    auto RenderPipeline::operator=(const RenderPipeline &other) -> RenderPipeline &
    {
        if (this != &other)
        {
            if (m_wgpu_render_pipeline != nullptr)
            {
                wgpuRenderPipelineRelease(m_wgpu_render_pipeline);
            }

            m_wgpu_render_pipeline = other.m_wgpu_render_pipeline;
            if (m_wgpu_render_pipeline != nullptr)
            {
                wgpuRenderPipelineAddRef(m_wgpu_render_pipeline);
            }
        }

        return *this;
    }

    auto RenderPipeline::operator=(RenderPipeline &&other) noexcept -> RenderPipeline &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_render_pipeline, other.m_wgpu_render_pipeline);
        }

        return *this;
    }

    auto RenderPipeline::get_raw_ptr() const -> WGPURenderPipeline
    {
        return m_wgpu_render_pipeline;
    }
}
