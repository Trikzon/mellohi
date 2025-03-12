#pragma once

#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/graphics.hpp"
#include "mellohi/graphics/vulkan/vulkan_device.hpp"

namespace mellohi
{
    struct Graphics::ImplData
    {
        std::unique_ptr<VulkanDevice> device;
    };
}

#endif

