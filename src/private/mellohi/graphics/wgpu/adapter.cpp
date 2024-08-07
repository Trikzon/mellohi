#include "mellohi/graphics/wgpu/adapter.hpp"

#include "mellohi/core/log.hpp"
#include "mellohi/core/types.hpp"

namespace mellohi::wgpu
{
    Adapter::Adapter(const Instance &instance, const Surface &surface)
    {
        const WGPURequestAdapterOptions request_adapter_options
        {
            .nextInChain = nullptr,
            .compatibleSurface = surface.get_raw_ptr(),
            // .powerPreference = WGPUPowerPreference_HighPerformance,
            // .backendType = ,
            // .forceFallbackAdapter = ,
            // .compatibilityMode = ,
        };

        struct UserData
        {
            WGPUAdapter adapter = nullptr;
            bool done = false;
        };
        UserData user_data{};

        auto callback = [](const WGPURequestAdapterStatus status, const WGPUAdapter adapter, const char *message,
                           void *user_data_ptr)
        {
            MH_ASSERT(status == WGPURequestAdapterStatus_Success, "Failed to request WGPU adapter: {}", message);

            auto *user_data = static_cast<UserData *>(user_data_ptr);
            user_data->adapter = adapter;
            user_data->done = true;
        };

        wgpuInstanceRequestAdapter(instance.get_raw_ptr(), &request_adapter_options, callback, &user_data);

        MH_ASSERT(user_data.done && user_data.adapter != nullptr, "Failed to request WGPU adapter.");

        m_wgpu_adapter = user_data.adapter;
    }

    Adapter::~Adapter()
    {
        if (m_wgpu_adapter != nullptr)
        {
            wgpuAdapterRelease(m_wgpu_adapter);
            m_wgpu_adapter = nullptr;
        }
    }

    Adapter::Adapter(const Adapter &other)
    {
        m_wgpu_adapter = other.m_wgpu_adapter;
        if (m_wgpu_adapter != nullptr)
        {
            wgpuAdapterAddRef(m_wgpu_adapter);
        }
    }

    Adapter::Adapter(Adapter &&other) noexcept
    {
        std::swap(other.m_wgpu_adapter, m_wgpu_adapter);
    }

    auto Adapter::operator=(const Adapter &other) -> Adapter &
    {
        if (this != &other)
        {
            if (m_wgpu_adapter != nullptr)
            {
                wgpuAdapterRelease(m_wgpu_adapter);
            }

            m_wgpu_adapter = other.m_wgpu_adapter;
            if (m_wgpu_adapter != nullptr)
            {
                wgpuAdapterAddRef(m_wgpu_adapter);
            }
        }

        return *this;
    }

    auto Adapter::operator=(Adapter &&other) noexcept -> Adapter &
    {
        if (this != &other)
        {
            std::swap(other.m_wgpu_adapter, m_wgpu_adapter);
        }

        return *this;
    }

    auto Adapter::get_limits() const -> WGPULimits
    {
        WGPUSupportedLimits supported_limits{};
        wgpuAdapterGetLimits(m_wgpu_adapter, &supported_limits);

        return supported_limits.limits;
    }

    auto Adapter::get_properties() const -> WGPUAdapterProperties
    {
        WGPUAdapterProperties adapter_properties{};
        wgpuAdapterGetProperties(m_wgpu_adapter, &adapter_properties);

        return adapter_properties;
    }


    auto Adapter::get_raw_ptr() const -> WGPUAdapter
    {
        return m_wgpu_adapter;
    }
}
