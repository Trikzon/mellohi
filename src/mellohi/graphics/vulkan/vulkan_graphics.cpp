#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/graphics/vulkan/vulkan_graphics.hpp"

#include "mellohi/core/logger.hpp"
#include "mellohi/graphics/vulkan/vulkan_device.hpp"
#include "mellohi/graphics/vulkan/vulkan_render_pass.hpp"
#include "mellohi/graphics/vulkan/vulkan_swapchain.hpp"

namespace mellohi
{
    Graphics::Graphics()
    {
        m_data = std::make_unique<ImplData>();

        m_data->device = std::make_shared<VulkanDevice>();
        m_data->swapchain = std::make_shared<VulkanSwapchain>(m_data->device);
        m_data->render_pass = std::make_shared<VulkanRenderPass>(m_data->device, m_data->swapchain);

        MH_TRACE("Successfully initialized Vulkan graphics.");
    }

    Graphics::~Graphics()
    {

    }

    void Graphics::draw_frame()
    {
        if (m_data->render_pass->begin())
        {
            m_data->render_pass->end();
        }
    }
}

#endif
