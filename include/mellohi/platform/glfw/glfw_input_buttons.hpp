#pragma once

#ifdef MH_PLATFORM_GLFW

#include "mellohi/platform/input_buttons.hpp"

#include <GLFW/glfw3.h>

namespace mellohi
{
    Key to_mh_key(i32 glfw_key);
    i32 to_glfw_key(Key mh_key);

    MouseButton to_mh_mouse_button(i32 glfw_mouse_button);
    i32 to_glfw_mouse_button(MouseButton mh_mouse_button);

    ButtonModFlags to_mh_button_mod_flags(i32 glfw_button_mod_flags);
    i32 to_glfw_button_mod_flags(ButtonModFlags mh_button_mod_flags);

    ButtonAction to_mh_button_action(i32 glfw_button_action);
    i32 to_glfw_button_action(ButtonAction mh_button_action);
}

#endif

