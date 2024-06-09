#pragma once

#include <mellohi/nodes/node.h>

using namespace mellohi;

class DebugOverlayNode final : public Node
{
public:
    explicit DebugOverlayNode(const std::string_view& name = "DebugOverlayNode") : Node(name) { }

private:
    void on_draw_imgui() override;
};
