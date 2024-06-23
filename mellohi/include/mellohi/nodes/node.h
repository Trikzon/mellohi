#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "mellohi/graphics/render_pass.h"

namespace mellohi
{
    class Node
    {
    public:
        explicit Node(const std::string_view& name = "Node");
        virtual ~Node() = default;

        void add_child(const std::shared_ptr<Node>& child_node);

    private:
        std::string m_name;
        std::unordered_map<std::string, std::weak_ptr<Node>> m_name_to_child_map;
        std::vector<std::shared_ptr<Node>> m_children;

        void update(double delta_time);
        virtual void on_update(double delta_time) { }

        void render(RenderPass &render_pass, double delta_time);
        virtual void on_render(RenderPass &render_pass, double delta_time) { }

        friend class Game;
    };
}
