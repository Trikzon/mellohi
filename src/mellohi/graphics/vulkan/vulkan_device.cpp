#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/core/logger.hpp"
#include "mellohi/graphics/vulkan/vulkan_device.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace mellohi
{
    VulkanDevice::VulkanDevice(const Platform &platform)
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        // TODO: Use engine config.
        const char *app_name = "App";
        const char *engine_name = "Mellohi";

        const vk::ApplicationInfo app_info
        {
            .pApplicationName = app_name,
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = engine_name,
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };

        const auto extensions = get_required_instance_extensions(platform);
        const auto validation_layers = get_required_validation_layers();

        vk::InstanceCreateFlagBits flags = {};
#ifdef __APPLE__
        flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

        // TODO: debug messenger.
        const void *next_ptr = nullptr;

        const vk::InstanceCreateInfo instance_ci
        {
            .pNext = next_ptr,
            .flags = flags,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<u32>(validation_layers.size()),
            .ppEnabledLayerNames = validation_layers.data(),
            .enabledExtensionCount = static_cast<u32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        const auto instance_resval = vk::createInstance(instance_ci);
        MH_ASSERT_VK(instance_resval.result, "Failed to create Vulkan instance.");
        m_instance = instance_resval.value;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
    }

    VulkanDevice::~VulkanDevice()
    {
        m_instance.destroy();
    }

    std::vector<const char *> VulkanDevice::get_required_instance_extensions(const Platform &platform) const
    {
        auto extensions = platform.get_required_vulkan_instance_extensions();

#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

#ifdef MH_DEBUG_MODE
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

    std::vector<const char *> VulkanDevice::get_required_device_extensions() const
    {
        std::vector<const char *> extensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

#ifdef __APPLE__
        extensions.push_back("VK_KHR_portability_subset");
#endif

        return extensions;
    }

    std::vector<const char *> VulkanDevice::get_required_validation_layers() const
    {
        std::vector<const char *> validation_layers;

#ifdef MH_DEBUG_MODE
        validation_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        return validation_layers;
    }
}

#endif

