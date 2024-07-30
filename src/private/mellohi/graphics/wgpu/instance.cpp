#include "mellohi/graphics/wgpu/instance.hpp"

#include "mellohi/core.hpp"

namespace mellohi::wgpu
{
    Instance::Instance()
    {
        const char *toggle_name = "enable_immedate_error_handling";
        const WGPUDawnTogglesDescriptor dawn_toggles
        {
            .chain = {
                .next = nullptr,
                .sType = WGPUSType_DawnTogglesDescriptor,
            },
            .enabledToggleCount = 1,
            .enabledToggles = &toggle_name,
            .disabledToggleCount = 0,
            .disabledToggles = nullptr,
        };

        const WGPUInstanceDescriptor instance_descriptor
        {
            .nextInChain = &dawn_toggles.chain,
            .features = nullptr,
        };

        m_wgpu_instance = wgpuCreateInstance(&instance_descriptor);
        MH_ASSERT(m_wgpu_instance != nullptr, "Failed to create WGPU instance.");
    }

    Instance::~Instance()
    {
        if (m_wgpu_instance != nullptr)
        {
            wgpuInstanceRelease(m_wgpu_instance);
            m_wgpu_instance = nullptr;
        }
    }

    Instance::Instance(const Instance &other)
    {
        m_wgpu_instance = other.m_wgpu_instance;
        if (m_wgpu_instance != nullptr)
        {
            wgpuInstanceAddRef(m_wgpu_instance);
        }
    }

    Instance::Instance(Instance &&other) noexcept
    {
        std::swap(other.m_wgpu_instance, m_wgpu_instance);
    }

    auto Instance::operator=(const Instance &other) -> Instance &
    {
        if (this != &other)
        {
            if (m_wgpu_instance != nullptr)
            {
                wgpuInstanceRelease(m_wgpu_instance);
            }

            m_wgpu_instance = other.m_wgpu_instance;
            if (m_wgpu_instance != nullptr)
            {
                wgpuInstanceAddRef(m_wgpu_instance);
            }
        }

        return *this;
    }

    auto Instance::operator=(Instance &&other) noexcept -> Instance &
    {
        if (this != &other)
        {
            std::swap(other.m_wgpu_instance, m_wgpu_instance);
        }

        return *this;
    }

    auto Instance::get_raw_ptr() const -> WGPUInstance
    {
        return m_wgpu_instance;
    }
}
