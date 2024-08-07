#pragma once

#include <GLFW/glfw3.h>

#include "mellohi/core/buttons.hpp"
#include "mellohi/core/types.hpp"

namespace mellohi::glfw
{
    class Window
    {
    public:
        using FramebufferSizeCallback = std::function<void(vec2u framebuffer_size)>;
        using KeyboardButtonCallback = std::function<void(KeyboardButton button, ButtonAction action,
                                                          ButtonModifier modifier)>;
        using MouseButtonCallback = std::function<void(MouseButton button, ButtonAction action,
                                                       ButtonModifier modifier)>;
        using CursorPosCallback = std::function<void(vec2f cursor_pos)>;

        Window(str title, vec2u initial_size);

        auto should_close() const -> bool;

        auto get_title() const -> string;
        auto set_title(str title) const -> void;

        auto get_pos() const -> vec2i;
        auto set_pos(vec2i pos) const -> void;

        auto get_size() const -> vec2u;
        auto set_size(vec2u size) const -> void;

        auto get_framebuffer_size() const -> vec2u;

        auto get_opacity() const -> f32;
        auto set_opacity(f32 opacity) const -> void;

        auto set_framebuffer_size_callback(const FramebufferSizeCallback &callback) const -> void;
        auto set_keyboard_button_callback(const KeyboardButtonCallback &callback) const -> void;
        auto set_mouse_button_callback(const MouseButtonCallback &callback) const -> void;
        auto set_cursor_pos_callback(const CursorPosCallback &callback) const -> void;

        auto get_raw_ptr() const -> GLFWwindow *;

    private:
        class Handle
        {
        public:
            explicit Handle(GLFWwindow *glfw_window);
            ~Handle();
            Handle(const Handle &other) = delete;
            Handle(Handle &&other) = delete;
            auto operator=(const Handle &other) -> Handle & = delete;
            auto operator=(Handle &&other) -> Handle & = delete;

            operator GLFWwindow *() const;

            auto set_framebuffer_size_callback(const FramebufferSizeCallback &callback) -> void;
            auto set_keyboard_button_callback(const KeyboardButtonCallback &callback) -> void;
            auto set_mouse_button_callback(const MouseButtonCallback &callback) -> void;
            auto set_cursor_pos_callback(const CursorPosCallback &callback) -> void;

        private:
            GLFWwindow *m_glfw_window{nullptr};
            FramebufferSizeCallback m_framebuffer_size_callback{nullptr};
            KeyboardButtonCallback m_keyboard_button_callback{nullptr};
            MouseButtonCallback m_mouse_button_callback{nullptr};
            CursorPosCallback m_cursor_pos_callback{nullptr};
        };

        s_ptr<Handle> m_handle{nullptr};
    };
}
