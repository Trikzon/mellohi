#ifdef MH_PLATFORM_GLFW

#include "mellohi/platform/glfw/glfw_platform.hpp"

#include "mellohi/core/logger.hpp"
#include "mellohi/platform/glfw/glfw.hpp"

namespace mellohi
{
    Platform::Platform()
    {
        m_data = std::make_unique<ImplData>();

        Glfw::get();

        MH_TRACE("Successfully initialized GLFW platform.");
    }

    Platform::~Platform()
    {
    }

    std::shared_ptr<Window> Platform::create_window(const uvec2 size, const bool resizable, const std::string &title)
    {
        auto window = std::shared_ptr<Window>{new Window{size, resizable, title}};
        window->initialize();
        return window;
    }

    void Platform::process_events()
    {
        Glfw::get().poll_events();
    }

    std::vector<const char *> Platform::get_required_vulkan_instance_extensions() const
    {
        u32 extension_count;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extension_count);

        if (!extensions)
        {
            return {};
        }

        return {extensions, extensions + extension_count};
    }
}

#endif

