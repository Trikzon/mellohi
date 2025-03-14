#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/core/config_assets.hpp"
#include "mellohi/core/engine.hpp"
#include "mellohi/graphics/vulkan/vulkan_device.hpp"
#include "mellohi/platform/platform.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace mellohi
{
    std::string_view to_string(VulkanQueueCapability capability)
    {
        switch (capability)
        {
            case VulkanQueueCapability::Graphics: return "Graphics";
            case VulkanQueueCapability::Present:  return "Present";
            default:                              return "Invalid";
        }
    }

    std::ostream & operator<<(std::ostream &os, VulkanQueueCapability capability)
    {
        return os << "VulkanQueueCapability(" << to_string(capability) << ")";
    }

    VulkanDevice::VulkanDevice()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
        create_instance();
        // TODO: Remove dependency on main window to allow for drawing on multiple windows.
        m_surface = Engine::get().main_window().create_vulkan_surface(m_instance);
        choose_physical_device();
        create_logical_device_and_queues();
    }

    VulkanDevice::~VulkanDevice()
    {
        m_logical_device.destroy();

        m_instance.destroySurfaceKHR(m_surface);

        if (m_debug_utils_messenger != nullptr)
        {
            m_instance.destroyDebugUtilsMessengerEXT(m_debug_utils_messenger);
        }

        m_instance.destroy();
    }

    vk::Queue VulkanDevice::get_queue(VulkanQueueCapability capability) const
    {
        const auto queue_it = m_queues.find(get_queue_family_index(capability));
        MH_ASSERT(queue_it != m_queues.end(), "Device is missing queue for {} capability.", capability);

        return queue_it->second;
    }

    u32 VulkanDevice::get_queue_family_index(VulkanQueueCapability capability) const
    {
        const auto queue_family_it = m_queue_family_indices.find(capability);
        MH_ASSERT(queue_family_it != m_queue_family_indices.end(), "Device is missing queue family for {} capability.", capability);

        return queue_family_it->second;
    }

    std::vector<u32> VulkanDevice::get_unique_queue_family_indices() const
    {
        const auto graphics_index = get_queue_family_index(VulkanQueueCapability::Graphics);
        const auto present_index = get_queue_family_index(VulkanQueueCapability::Present);

        if (graphics_index != present_index)
        {
            return {graphics_index, present_index};
        }
        else
        {
            return {graphics_index};
        }
    }

    static vk::Bool32 vk_debug_callback(const vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                        const vk::DebugUtilsMessageTypeFlagsEXT message_types,
                                        const vk::DebugUtilsMessengerCallbackDataEXT *callback_data_ptr,
                                        void *user_data)
    {
        switch (message_severity)
        {
        default:
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            MH_ERROR(callback_data_ptr->pMessage);
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            MH_WARN(callback_data_ptr->pMessage);
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            MH_INFO(callback_data_ptr->pMessage);
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            MH_TRACE(callback_data_ptr->pMessage);
            break;
        }

        return VK_FALSE;
    };

    static constexpr vk::DebugUtilsMessengerCreateInfoEXT DEBUG_UTILS_MESSENGER_CI
    {
        .flags = {},
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = vk_debug_callback,
        .pUserData = {},
    };

    void VulkanDevice::create_instance()
    {
        const auto &engine_config = Engine::get().engine_config();

        const auto game_title = engine_config.game_title();
        const auto engine_title = engine_config.engine_title();

        const vk::ApplicationInfo app_info
        {
            .pApplicationName = game_title.c_str(),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = engine_title.c_str(),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_4,
        };

        const auto extensions = get_required_instance_extensions();
        const auto validation_layers = get_required_validation_layers();

        vk::InstanceCreateFlagBits flags = {};
#ifdef __APPLE__
        flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

        const void *next_ptr = nullptr;
#ifdef MH_DEBUG_MODE
        next_ptr = &DEBUG_UTILS_MESSENGER_CI;
#endif

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

#ifdef MH_DEBUG_MODE
        const auto debug_utils_messenger_resval = m_instance.createDebugUtilsMessengerEXT(DEBUG_UTILS_MESSENGER_CI);
        MH_ASSERT_VK(debug_utils_messenger_resval.result, "Failed to create Vulkan debug utils messenger.");
        m_debug_utils_messenger = debug_utils_messenger_resval.value;
#endif
    }

    void VulkanDevice::choose_physical_device()
    {
        const auto physical_devices_resval = m_instance.enumeratePhysicalDevices();
        MH_ASSERT_VK(physical_devices_resval.result, "Failed to enumerate Vulkan physical devices.");
        const auto physical_devices = physical_devices_resval.value;

        MH_ASSERT(!physical_devices.empty(), "Failed to find GPUs with Vulkan support.");

        for (const auto &physical_device : physical_devices)
        {
            if (physical_device.getSurfaceFormatsKHR(m_surface).value.empty())
            {
                continue;
            }

            if (physical_device.getSurfacePresentModesKHR(m_surface).value.empty())
            {
                continue;
            }

            const auto queue_families = physical_device.getQueueFamilyProperties();
            std::unordered_map<VulkanQueueCapability, u32> queue_family_indices;
            for (auto i = 0; i < queue_families.size(); ++i)
            {
                const auto &queue_family = queue_families[i];
                if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    queue_family_indices.try_emplace(VulkanQueueCapability::Graphics, i);
                }

                VkBool32 present_support = false;
                const auto _ = physical_device.getSurfaceSupportKHR(i, m_surface, &present_support);
                if (present_support)
                {
                    queue_family_indices.try_emplace(VulkanQueueCapability::Present, i);
                }
            }

            if (queue_family_indices.contains(VulkanQueueCapability::Graphics)
                && queue_family_indices.contains(VulkanQueueCapability::Present))
            {
                m_queue_family_indices = queue_family_indices;
                m_physical_device = physical_device;

                const auto properties = m_physical_device.getProperties();
                MH_INFO("Selected GPU {}.", properties.deviceName);

                break;
            }
        }

        MH_ASSERT(m_physical_device != nullptr, "Failed to find a suitable GPU.");
    }

    void VulkanDevice::create_logical_device_and_queues()
    {
        std::vector<vk::DeviceQueueCreateInfo> device_queue_ci;
        const f32 queue_priority = 1.0f;
        for (const u32 queue_family_index : get_unique_queue_family_indices())
        {
            device_queue_ci.push_back(vk::DeviceQueueCreateInfo
            {
                .queueFamilyIndex = queue_family_index,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority,
            });
        }

        const auto extensions = get_required_device_extensions();
        const auto validation_layers = get_required_validation_layers();

        const vk::PhysicalDeviceFeatures physical_device_features;

        const vk::DeviceCreateInfo logical_device_ci
        {
            .queueCreateInfoCount = static_cast<u32>(device_queue_ci.size()),
            .pQueueCreateInfos = device_queue_ci.data(),
            .enabledLayerCount = static_cast<u32>(validation_layers.size()),
            .ppEnabledLayerNames = validation_layers.data(),
            .enabledExtensionCount = static_cast<u32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = &physical_device_features,
        };

        const auto logical_device_resval = m_physical_device.createDevice(logical_device_ci);
        MH_ASSERT_VK(logical_device_resval.result, "Failed to create Vulkan logical device.");
        m_logical_device = logical_device_resval.value;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_logical_device);

        for (const u32 queue_family_index : get_unique_queue_family_indices())
        {
            vk::Queue queue;
            m_logical_device.getQueue(queue_family_index, 0, &queue);
            m_queues.try_emplace(queue_family_index, queue);
        }
    }

    std::vector<const char *> VulkanDevice::get_required_instance_extensions() const
    {
        auto extensions = Engine::get().platform().get_required_vulkan_instance_extensions();

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

