#pragma once

#include <GLFW/glfw3.h>

#include "mellohi/core.hpp"

namespace mellohi::glfw
{
    class Window
    {
    public:
        using FramebufferSizeCallback = std::function<void(vec2u)>;

        Window(str title, vec2u size);

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

            // ReSharper disable once CppNonExplicitConversionOperator
            operator GLFWwindow *() const;

            auto set_framebuffer_size_callback(const FramebufferSizeCallback &callback) -> void;

        private:
            GLFWwindow *m_glfw_window{nullptr};
            FramebufferSizeCallback m_framebuffer_size_callback{nullptr};
        };

        s_ptr<Handle> m_handle{nullptr};
    };
}
