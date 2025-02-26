#include "GLFW/glfw3.h"
#ifdef MH_PLATFORM_GLFW

#include "mellohi/platform/glfw/glfw.hpp"
#include "mellohi/platform/glfw/glfw_window.hpp"

namespace mellohi
{
    Window::~Window()
    {
        Glfw::get().destroy_window(m_data->glfw_window);
    }
    
    bool Window::should_close() const
    {
        return Glfw::get().window_should_close(m_data->glfw_window);
    }
    
    Window::Window(const uvec2 size, bool resizable, const std::string &title)
    {
        m_data = std::make_unique<ImplData>();
        
        auto &glfw = Glfw::get();
        
        glfw.window_hint(GLFW_RESIZABLE, resizable);
        
        #ifdef MH_GRAPHICS_VULKAN
            glfw.window_hint(GLFW_CLIENT_API, GLFW_NO_API);
        #endif
        
        m_data->glfw_window = glfw.create_window(size, title);
    }
}

#endif
