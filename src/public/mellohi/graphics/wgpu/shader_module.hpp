#pragma once

#include "mellohi/graphics/wgpu/device.hpp"

namespace mellohi::wgpu
{
    class ShaderModule
    {
    public:
        ShaderModule(const Device &device, const AssetId &wgsl_file_id);
        ~ShaderModule();
        ShaderModule(const ShaderModule &other);
        ShaderModule(ShaderModule &&other) noexcept;
        auto operator=(const ShaderModule &other) -> ShaderModule &;
        auto operator=(ShaderModule &&other) noexcept -> ShaderModule &;

        auto get_raw_ptr() const -> WGPUShaderModule;

    private:
        WGPUShaderModule m_wgpu_shader_module{nullptr};
    };
}
