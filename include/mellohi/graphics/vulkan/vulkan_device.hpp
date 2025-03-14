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

        vk::Queue get_queue(VulkanQueueCapability capability) const;
        u32 get_queue_family_index(VulkanQueueCapability capability) const;
        std::vector<u32> get_unique_queue_family_indices() const;

    private:
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_utils_messenger;
        vk::SurfaceKHR m_surface;
        vk::PhysicalDevice m_physical_device;
        vk::Device m_logical_device;
        std::unordered_map<VulkanQueueCapability, u32> m_queue_family_indices;
        std::unordered_map<u32, vk::Queue> m_queues;

        void create_instance();
        void choose_physical_device();
        void create_logical_device_and_queues();

        std::vector<const char *> get_required_instance_extensions() const;
        std::vector<const char *> get_required_device_extensions() const;
        std::vector<const char *> get_required_validation_layers() const;
    };
}

#endif

