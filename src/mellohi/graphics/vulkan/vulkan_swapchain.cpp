#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan_swapchain.hpp"

#include "mellohi/core/config_assets.hpp"
#include "mellohi/core/engine.hpp"
#include "mellohi/graphics/vulkan/vulkan_device.hpp"
#include "mellohi/graphics/vulkan/vulkan_render_pass.hpp"
#include "mellohi/platform/window.hpp"

namespace mellohi
{
    VulkanSwapchain::VulkanSwapchain(std::shared_ptr<VulkanDevice> device) : m_device(device)
    {
        create_swapchain();
        create_image_views();
        create_sync_objects();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        destroy();

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_device->destroy_fence(m_in_flight_fences[i]);
            m_device->destroy_semaphore(m_render_finished_semaphores[i]);
            m_device->destroy_semaphore(m_image_available_semaphores[i]);
        }
    }

    void VulkanSwapchain::initialize(const vk::RenderPass render_pass)
    {
        m_render_pass = std::move(render_pass);

        create_framebuffers();
    }

    std::optional<vk::Framebuffer> VulkanSwapchain::acquire_next_framebuffer()
    {
        MH_ASSERT(!m_current_framebuffer_index.has_value(), "Cannot acquire the next framebuffer before presenting.");

        m_device->wait_for_fence(m_in_flight_fences[m_current_frame_index]);

        u32 framebuffer_index;
        const auto result = m_device->logical_device()
            .acquireNextImageKHR(m_swapchain, std::numeric_limits<u64>::max(),
                                 m_image_available_semaphores[m_current_frame_index], {}, &framebuffer_index);

        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            recreate();
            return std::nullopt;
        }
        else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
        {
            MH_PANIC("Failed to acquire Vulkan swapchain image.");
        }

        m_device->reset_fence(m_in_flight_fences[m_current_frame_index]);

        m_current_framebuffer_index = framebuffer_index;

        return m_framebuffers[framebuffer_index];
    }

    void VulkanSwapchain::present(vk::CommandBuffer command_buffer)
    {
        MH_ASSERT(m_current_framebuffer_index.has_value(), "Cannot present before acquiring next framebuffer.");

        const vk::Semaphore wait_semaphores[] = {m_image_available_semaphores[m_current_frame_index]};
        const vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        const vk::Semaphore signal_semaphores[] = {m_render_finished_semaphores[m_current_frame_index]};

        const vk::SubmitInfo submit_info
        {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = wait_semaphores,
            .pWaitDstStageMask = wait_stages,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signal_semaphores,
        };

        const auto graphics_queue = m_device->queue(VulkanQueueCapability::Graphics);

        auto result = graphics_queue.submit(1, &submit_info, m_in_flight_fences[m_current_frame_index]);
        MH_ASSERT_VK(result, "Failed to submit Vulkan queue.");

        u32 framebuffer_index = m_current_framebuffer_index.value();

        const vk::PresentInfoKHR present_info
        {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signal_semaphores,
            .swapchainCount = 1,
            .pSwapchains = &m_swapchain,
            .pImageIndices = &framebuffer_index,
            .pResults = nullptr,
        };

        result = graphics_queue.presentKHR(present_info);
        if (m_should_be_recreated || result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            recreate();
        }
        else if (result != vk::Result::eSuccess)
        {
            MH_PANIC("Failed to present Vulkan swapchain image.");
        }

        m_current_frame_index = (m_current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
        m_current_framebuffer_index = std::nullopt;
    }

    usize VulkanSwapchain::current_frame_index() const
    {
        return m_current_frame_index;
    }

    vk::Extent2D VulkanSwapchain::extent() const
    {
        return m_extent;
    }

    void VulkanSwapchain::create_swapchain()
    {
        const auto &engine_config = Engine::get().engine_config();

        const auto available_present_modes = m_device->surface_present_modes();
        auto present_mode = vk::PresentModeKHR::eFifo;  // Guaranteed to be supported.
        for (const auto &available_present_mode : available_present_modes)
        {
            if (engine_config.window_vsync() && available_present_mode == vk::PresentModeKHR::eMailbox)
            {
                present_mode = available_present_mode;
                break;
            }

            if (!engine_config.window_vsync() && available_present_mode == vk::PresentModeKHR::eImmediate)
            {
                present_mode = available_present_mode;
                break;
            }
        }

        const auto surface_capabilities = m_device->surface_capabilities();
        m_extent = surface_capabilities.currentExtent;
        if (m_extent.width == std::numeric_limits<u32>::max())
        {
            // TODO: Remove dependency on main window to allow for drawing on multiple windows.
            const auto framebuffer_size = Engine::get().main_window().framebuffer_size();
            m_extent.width = std::clamp(framebuffer_size.x,
                                        surface_capabilities.minImageExtent.width,
                                        surface_capabilities.maxImageExtent.width);
            m_extent.height = std::clamp(framebuffer_size.y,
                                         surface_capabilities.minImageExtent.height,
                                         surface_capabilities.maxImageExtent.height);
        }

        auto min_image_count = surface_capabilities.minImageCount + 1;
        if (surface_capabilities.maxImageCount > 0)
        {
            min_image_count = std::min(min_image_count, surface_capabilities.maxImageCount);
        }

        const auto surface_format = m_device->preferred_surface_format();

        vk::SwapchainCreateInfoKHR swapchain_ci
        {
            .surface = m_device->surface(),
            .minImageCount = min_image_count,
            .imageFormat = surface_format.format,
            .imageColorSpace = surface_format.colorSpace,
            .imageExtent = m_extent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .preTransform = surface_capabilities.currentTransform,
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode = present_mode,
            .clipped = vk::True,
        };

        const auto queue_family_indices = m_device->unique_queue_family_indices();
        if (queue_family_indices.size() == 1)
        {
            swapchain_ci.imageSharingMode = vk::SharingMode::eExclusive;
        }
        else
        {
            swapchain_ci.imageSharingMode = vk::SharingMode::eConcurrent;
            swapchain_ci.queueFamilyIndexCount = queue_family_indices.size();
            swapchain_ci.pQueueFamilyIndices = queue_family_indices.data();
        }

        m_swapchain = m_device->create_swapchain(swapchain_ci);
    }

    void VulkanSwapchain::create_image_views()
    {
        const auto swapchain_images = m_device->swapchain_images(m_swapchain);
        const auto surface_format = m_device->preferred_surface_format();

        for (const auto &swapchain_image : swapchain_images)
        {
            const vk::ImageViewCreateInfo image_view_ci
            {
                .image = swapchain_image,
                .viewType = vk::ImageViewType::e2D,
                .format = surface_format.format,
                .components = vk::ComponentMapping
                {
                    .r = vk::ComponentSwizzle::eIdentity,
                    .g = vk::ComponentSwizzle::eIdentity,
                    .b = vk::ComponentSwizzle::eIdentity,
                    .a = vk::ComponentSwizzle::eIdentity,
                },
                .subresourceRange = vk::ImageSubresourceRange
                {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };

            m_image_views.push_back(m_device->create_image_view(image_view_ci));
        }
    }

    void VulkanSwapchain::create_framebuffers()
    {
        MH_ASSERT(m_render_pass != nullptr, "Swapchain cannot create framebuffers without being initialized.");

        for (const auto &image_view : m_image_views)
        {
            const vk::FramebufferCreateInfo framebuffer_ci
            {
                .renderPass = m_render_pass,
                .attachmentCount = 1,
                .pAttachments = &image_view,
                .width = m_extent.width,
                .height = m_extent.height,
                .layers = 1,
            };

            m_framebuffers.push_back(m_device->create_framebuffer(framebuffer_ci));
        }
    }

    void VulkanSwapchain::create_sync_objects()
    {
        const vk::FenceCreateInfo fence_ci
        {
            .flags = vk::FenceCreateFlagBits::eSignaled,
        };

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_image_available_semaphores.push_back(m_device->create_semaphore({}));
            m_render_finished_semaphores.push_back(m_device->create_semaphore({}));
            m_in_flight_fences.push_back(m_device->create_fence(fence_ci));
        }
    }

    void VulkanSwapchain::recreate()
    {
        destroy();
        create_swapchain();
        create_image_views();
        create_framebuffers();

        m_should_be_recreated = false;
    }

    void VulkanSwapchain::destroy()
    {
        m_device->wait_idle();

        for (const auto &framebuffer : m_framebuffers)
        {
            m_device->destroy_framebuffer(framebuffer);
        }
        m_framebuffers.clear();

        for (const auto &image_view : m_image_views)
        {
            m_device->destroy_image_view(image_view);
        }
        m_image_views.clear();

        m_device->destroy_swapchain(m_swapchain);
        m_swapchain = nullptr;
    }
}

#endif
