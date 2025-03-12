#pragma once

#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan.hpp"
#include "mellohi/platform/platform.hpp"

namespace mellohi
{
    class VulkanDevice
    {
    public:
        VulkanDevice(const Platform &platform);
        ~VulkanDevice();

    private:
        vk::Instance m_instance;

        std::vector<const char *> get_required_instance_extensions(const Platform &platform) const;
        std::vector<const char *> get_required_device_extensions() const;
        std::vector<const char *> get_required_validation_layers() const;
    };
}

#endif

