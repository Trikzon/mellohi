#pragma once

#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan.hpp"

namespace mellohi
{
    class VulkanDevice;
    class VulkanSwapchain;

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapchain> swapchain);
        ~VulkanRenderPass();

        bool begin();
        void end();

        vk::CommandBuffer current_command_buffer() const;
        vk::RenderPass render_pass() const;

    private:
        std::shared_ptr<VulkanDevice> m_device;
        std::shared_ptr<VulkanSwapchain> m_swapchain;
        vk::RenderPass m_render_pass;
        vk::CommandPool m_command_pool;
        std::vector<vk::CommandBuffer> m_command_buffers;

        void create_render_pass();
        void create_command_pool();
    };
}

#endif

