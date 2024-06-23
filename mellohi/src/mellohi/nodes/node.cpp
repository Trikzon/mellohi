#include "mellohi/nodes/node.h"

namespace mellohi
{
    Node::Node(const std::string_view &name) : m_name(name) { }

    void Node::add_child(const std::shared_ptr<Node> &child_node)
    {
        // TODO: Assert that child_node is not null.
        // TODO: Assert that this child_node != this
        // TODO: Assert that this does not already have a child with the same name.

        m_name_to_child_map.insert({ child_node->m_name, child_node });
        m_children.push_back(child_node);
    }

    void Node::update(const double delta_time)
    {
        for (const auto &child : m_children)
        {
            child->update(delta_time);
        }

        on_update(delta_time);
    }

    void Node::render(RenderPass &render_pass, const double delta_time)
    {
        for (const auto &child : m_children)
        {
            child->render(render_pass, delta_time);
        }

        on_render(render_pass, delta_time);
    }
}
