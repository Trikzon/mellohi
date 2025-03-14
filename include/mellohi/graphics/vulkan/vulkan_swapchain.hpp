#pragma once

#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan.hpp"

namespace mellohi
{
    class VulkanDevice;

    class VulkanSwapchain
    {
    public:
        const static usize MAX_FRAMES_IN_FLIGHT = 2;

        explicit VulkanSwapchain(std::shared_ptr<VulkanDevice> device);
        ~VulkanSwapchain();

        void initialize(vk::RenderPass render_pass);

        std::optional<vk::Framebuffer> acquire_next_framebuffer();
        void present(vk::CommandBuffer command_buffer);

        usize current_frame_index() const;
        vk::Extent2D extent() const;

    private:
        std::shared_ptr<VulkanDevice> m_device;
        vk::RenderPass m_render_pass;
        vk::SwapchainKHR m_swapchain;
        vk::Extent2D m_extent;
        std::vector<vk::ImageView> m_image_views;
        std::vector<vk::Framebuffer> m_framebuffers;
        std::vector<vk::Semaphore> m_image_available_semaphores;
        std::vector<vk::Semaphore> m_render_finished_semaphores;
        std::vector<vk::Fence> m_in_flight_fences;
        usize m_current_frame_index = 0;
        bool m_should_be_recreated = false;
        std::optional<u32> m_current_framebuffer_index;

        void create_swapchain();
        void create_image_views();
        void create_framebuffers();
        void create_sync_objects();

        void recreate();
        void destroy();
    };
}

#endif

