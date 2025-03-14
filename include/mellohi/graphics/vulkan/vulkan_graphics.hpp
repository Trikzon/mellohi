#pragma once

#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/graphics.hpp"

namespace mellohi
{
    class VulkanDevice;
    class VulkanRenderPass;
    class VulkanSwapchain;

    struct Graphics::ImplData
    {
        std::shared_ptr<VulkanDevice> device;
        std::shared_ptr<VulkanSwapchain> swapchain;
        std::shared_ptr<VulkanRenderPass> render_pass;
    };
}

#endif

