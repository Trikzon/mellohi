#include "mellohi/graphics/glfw/glfw.hpp"

#include <GLFW/glfw3.h>

#include "mellohi/core.hpp"

namespace mellohi::glfw
{
    Glfw::~Glfw()
    {
        MH_INFO("Terminating GLFW.");
        glfwTerminate();
    }

    auto Glfw::get_instance() -> Glfw &
    {
        static Glfw instance;
        return instance;
    }

    auto Glfw::get_time() const -> f64
    {
        return glfwGetTime();
    }

    auto Glfw::poll_events() const -> void
    {
        glfwPollEvents();
    }

    Glfw::Glfw()
    {
        MH_INFO("Initializing GLFW.");
        MH_ASSERT(glfwInit(), "Failed to initialize GLFW.");
    }
}
