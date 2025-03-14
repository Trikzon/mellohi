#pragma once

#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan.hpp"

namespace mellohi
{
    enum class VulkanQueueCapability
    {
        Graphics,
        Present,
    };

    std::string_view to_string(VulkanQueueCapability capability);
    std::ostream & operator<<(std::ostream &os, VulkanQueueCapability capability);

    class VulkanDevice
    {
    public:
        VulkanDevice();
        ~VulkanDevice();

        void reset_fence(vk::Fence fence) const;
        void wait_for_fence(vk::Fence fence, u64 timeout = std::numeric_limits<u64>::max()) const;
        void wait_idle() const;

        std::vector<vk::CommandBuffer> allocate_command_buffers(
                const vk::CommandBufferAllocateInfo &allocate_info) const;
        vk::CommandPool create_command_pool(const vk::CommandPoolCreateInfo &create_info) const;
        vk::Fence create_fence(const vk::FenceCreateInfo &create_info) const;
        vk::Framebuffer create_framebuffer(const vk::FramebufferCreateInfo &create_info) const;
        vk::ImageView create_image_view(const vk::ImageViewCreateInfo &create_info) const;
        vk::RenderPass create_render_pass(const vk::RenderPassCreateInfo &create_info) const;
        vk::Semaphore create_semaphore(const vk::SemaphoreCreateInfo &create_info) const;
        vk::SwapchainKHR create_swapchain(const vk::SwapchainCreateInfoKHR &create_info) const;

        void destroy_command_pool(vk::CommandPool command_pool) const;
        void destroy_fence(vk::Fence fence) const;
        void destroy_framebuffer(vk::Framebuffer framebuffer) const;
        void destroy_image_view(vk::ImageView image_view) const;
        void destroy_render_pass(vk::RenderPass render_pass) const;
        void destroy_semaphore(vk::Semaphore semaphore) const;
        void destroy_swapchain(vk::SwapchainKHR swapchain) const;

        vk::Device logical_device() const;
        vk::SurfaceFormatKHR preferred_surface_format() const;
        vk::Queue queue(VulkanQueueCapability capability) const;
        u32 queue_family_index(VulkanQueueCapability capability) const;
        vk::SurfaceKHR surface() const;
        std::vector<vk::SurfaceFormatKHR> surface_formats() const;
        vk::SurfaceCapabilitiesKHR surface_capabilities() const;
        std::vector<vk::PresentModeKHR> surface_present_modes() const;
        std::vector<vk::Image> swapchain_images(vk::SwapchainKHR swapchain) const;
        std::vector<u32> unique_queue_family_indices() const;

    private:
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_utils_messenger;
        vk::SurfaceKHR m_surface;
        vk::PhysicalDevice m_physical_device;
        vk::Device m_logical_device;
        std::unordered_map<VulkanQueueCapability, u32> m_queue_family_indices;
        std::unordered_map<u32, vk::Queue> m_queues;
        vk::SurfaceFormatKHR m_preferred_surface_format;

        void create_instance();
        void choose_physical_device();
        void create_logical_device_and_queues();
        void choose_preferred_surface_format();

        std::vector<const char *> required_instance_extensions() const;
        std::vector<const char *> required_device_extensions() const;
        std::vector<const char *> required_validation_layers() const;
    };
}

#endif

