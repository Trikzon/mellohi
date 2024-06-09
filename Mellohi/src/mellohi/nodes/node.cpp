#include "mellohi/nodes/node.h"

#include <ranges>

#include "mellohi/log.h"

namespace mellohi
{
    Node::Node(const std::string_view& name) : m_name(name) { }

    void Node::add_child(const std::shared_ptr<Node>& child_node)
    {
        MH_ASSERT(child_node != nullptr, "Attempted to add null child to node (name = {}).", m_name);
        MH_ASSERT(child_node.get() != this, "Attempted to add node as a child to itself (name = {}).", m_name);
        MH_ASSERT(
            !m_children_idx_map.contains(child_node->m_name),
            "Attempted to add a second child node with the same name ({}) to node (name = {}).",
            child_node->m_name, m_name
        );

        m_children_idx_map.insert({ child_node->m_name, m_children.size() });
        m_children.push_back(child_node);
    }

    void Node::update()
    {
        for (const auto& child : m_children)
        {
            if (child != nullptr)
            {
                child->update();
            }
        }

        on_update();
    }

    void Node::draw_imgui()
    {
        for (const auto& child : m_children)
        {
            if (child != nullptr)
            {
                child->draw_imgui();
            }
        }

        on_draw_imgui();
    }
}
