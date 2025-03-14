#ifdef MH_GRAPHICS_VULKAN

#include "mellohi/core/logger.hpp"
#include "mellohi/graphics/vulkan/vulkan_graphics.hpp"

namespace mellohi
{
    Graphics::Graphics()
    {
        m_data = std::make_unique<ImplData>();

        m_data->device = std::make_unique<VulkanDevice>();

        MH_TRACE("Successfully initialized Vulkan graphics.");
    }

    Graphics::~Graphics()
    {
        MH_TRACE("Destructing graphics");
    }

    void Graphics::draw_frame()
    {

    }
}

#endif
