#include "mellohi/graphics/wgpu/shader_module.hpp"

namespace mellohi::wgpu
{
    ShaderModule::ShaderModule(const Device &device, const AssetId &wgsl_file_id)
    {
        const string wgsl_code = wgsl_file_id.read_file_as_string();

        const WGPUShaderModuleWGSLDescriptor shader_module_wgsl_descriptor
        {
            .chain = {
                .next = nullptr,
                .sType = WGPUSType_ShaderModuleWGSLDescriptor,
            },
            .code = wgsl_code.c_str(),
        };

        const string label = wgsl_file_id.get_fully_qualified_id();
        const WGPUShaderModuleDescriptor shader_module_descriptor
        {
            .nextInChain = &shader_module_wgsl_descriptor.chain,
            .label = label.c_str(),
        };

        m_wgpu_shader_module = wgpuDeviceCreateShaderModule(device.get_raw_ptr(), &shader_module_descriptor);
    }

    ShaderModule::~ShaderModule()
    {
        if (m_wgpu_shader_module != nullptr)
        {
            wgpuShaderModuleRelease(m_wgpu_shader_module);
            m_wgpu_shader_module = nullptr;
        }
    }

    ShaderModule::ShaderModule(const ShaderModule &other)
    {
        m_wgpu_shader_module = other.m_wgpu_shader_module;
        if (m_wgpu_shader_module != nullptr)
        {
            wgpuShaderModuleAddRef(m_wgpu_shader_module);
        }
    }

    ShaderModule::ShaderModule(ShaderModule &&other) noexcept
    {
        std::swap(m_wgpu_shader_module, other.m_wgpu_shader_module);
    }

    auto ShaderModule::operator=(const ShaderModule &other) -> ShaderModule &
    {
        if (this != &other)
        {
            if (m_wgpu_shader_module != nullptr)
            {
                wgpuShaderModuleRelease(m_wgpu_shader_module);
            }

            m_wgpu_shader_module = other.m_wgpu_shader_module;
            if (m_wgpu_shader_module != nullptr)
            {
                wgpuShaderModuleAddRef(m_wgpu_shader_module);
            }
        }

        return *this;
    }

    auto ShaderModule::operator=(ShaderModule &&other) noexcept -> ShaderModule &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_shader_module, other.m_wgpu_shader_module);
        }

        return *this;
    }

    auto ShaderModule::get_raw_ptr() const -> WGPUShaderModule
    {
        return m_wgpu_shader_module;
    }
}
