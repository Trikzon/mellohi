#pragma once

#include <webgpu/webgpu.h>

namespace mellohi::wgpu
{
    class Instance
    {
    public:
        Instance();
        ~Instance();
        Instance(const Instance &other);
        Instance(Instance &&other) noexcept;
        auto operator=(const Instance &other) -> Instance &;
        auto operator=(Instance &&other) noexcept -> Instance &;

        auto get_raw_ptr() const -> WGPUInstance;

    private:
        WGPUInstance m_wgpu_instance{nullptr};
    };
}
