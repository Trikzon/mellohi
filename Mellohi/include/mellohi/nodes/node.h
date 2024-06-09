#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "mellohi/types.h"

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
        std::unordered_map<std::string, usize> m_children_idx_map;
        std::vector<std::shared_ptr<Node>> m_children;

        void update();
        virtual void on_update() { }

        void draw_imgui();
        virtual void on_draw_imgui() { };

        friend class App;
    };
}
