#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan_render_pass.hpp"

#include "mellohi/core/config_assets.hpp"
#include "mellohi/core/engine.hpp"
#include "mellohi/graphics/vulkan/vulkan_device.hpp"
#include "mellohi/graphics/vulkan/vulkan_swapchain.hpp"

namespace mellohi
{
    VulkanRenderPass::VulkanRenderPass(const std::shared_ptr<VulkanDevice> device,
                                       const std::shared_ptr<VulkanSwapchain> swapchain)
        : m_device(std::move(device)), m_swapchain(std::move(swapchain))
    {
        create_render_pass();
        m_swapchain->initialize(m_render_pass);
        create_command_pool();
    }

    VulkanRenderPass::~VulkanRenderPass()
    {
        m_device->wait_idle();

        m_device->destroy_command_pool(m_command_pool);
        m_device->destroy_render_pass(m_render_pass);
    }

    bool VulkanRenderPass::begin()
    {
        const auto framebuffer = m_swapchain->acquire_next_framebuffer();

        if (!framebuffer.has_value())
        {
            return false;
        }

        const auto command_buffer = current_command_buffer();
        command_buffer.reset();

        const vk::CommandBufferBeginInfo command_buffer_begin_info
        {
            .flags = {},
            .pInheritanceInfo = nullptr,
        };

        const auto result = command_buffer.begin(command_buffer_begin_info);
        MH_ASSERT_VK(result, "Failed to begin recording Vulkan command buffer.");

        const vk::ClearValue clear_value
        {
            .color = vk::ClearColorValue
            {
                .float32 = Engine::get().engine_config().window_clear_color().srgb_to_linear().as_array(),
            },
        };

        const auto swapchain_extent = m_swapchain->extent();

        const vk::RenderPassBeginInfo render_pass_begin_info
        {
            .renderPass = m_render_pass,
            .framebuffer = framebuffer.value(),
            .renderArea = vk::Rect2D
            {
                .offset = {0, 0},
                .extent = swapchain_extent,
            },
            .clearValueCount = 1,
            .pClearValues = &clear_value,
        };

        command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

        const vk::Viewport viewport
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<f32>(swapchain_extent.width),
            .height = static_cast<f32>(swapchain_extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };
        command_buffer.setViewport(0, 1, &viewport);

        const vk::Rect2D scissor
        {
            .offset = {0, 0},
            .extent = swapchain_extent,
        };
        command_buffer.setScissor(0, 1, &scissor);

        return true;
    }

    void VulkanRenderPass::end()
    {
        const auto command_buffer = current_command_buffer();

        command_buffer.endRenderPass();

        const auto result = command_buffer.end();
        MH_ASSERT_VK(result, "Failed to end recording Vulkan command buffer.");

        m_swapchain->present(command_buffer);
    }

    vk::CommandBuffer VulkanRenderPass::current_command_buffer() const
    {
        return m_command_buffers[m_swapchain->current_frame_index()];
    }

    vk::RenderPass VulkanRenderPass::render_pass() const
    {
        return m_render_pass;
    }

    void VulkanRenderPass::create_render_pass()
    {
        const auto surface_format = m_device->preferred_surface_format();

        const vk::AttachmentDescription color_attachment
        {
            .format = surface_format.format,
            .samples = vk::SampleCountFlagBits::e1,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = vk::ImageLayout::eUndefined,
            .finalLayout = vk::ImageLayout::ePresentSrcKHR,
        };

        constexpr vk::AttachmentReference color_attachment_ref
        {
            .attachment = 0,
            .layout = vk::ImageLayout::eColorAttachmentOptimal,
        };

        const vk::SubpassDescription subpass_description
        {
            .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_ref,
        };

        constexpr vk::SubpassDependency subpass_dependency
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
            .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
            .srcAccessMask = {},
            .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
        };

        const vk::RenderPassCreateInfo render_pass_ci
        {
            .attachmentCount = 1,
            .pAttachments = &color_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass_description,
            .dependencyCount = 1,
            .pDependencies = &subpass_dependency,
        };

        m_render_pass = m_device->create_render_pass(render_pass_ci);
    }

    void VulkanRenderPass::create_command_pool()
    {
        const vk::CommandPoolCreateInfo command_pool_ci
        {
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = m_device->queue_family_index(VulkanQueueCapability::Graphics),
        };

        m_command_pool = m_device->create_command_pool(command_pool_ci);

        const vk::CommandBufferAllocateInfo command_buffer_allocate_info
        {
            .commandPool = m_command_pool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = static_cast<u32>(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT),
        };

        m_command_buffers = m_device->allocate_command_buffers(command_buffer_allocate_info);
    }
}

#endif
