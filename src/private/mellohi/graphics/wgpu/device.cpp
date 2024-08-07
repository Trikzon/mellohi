#include "mellohi/graphics/wgpu/device.hpp"

#include "mellohi/core/log.hpp"
#include "mellohi/core/types.hpp"

namespace mellohi::wgpu
{
    Device::Device(const Adapter &adapter, const WGPULimits &limits)
    {
        m_adapter = std::make_shared<Adapter>(adapter);

        const WGPURequiredLimits required_limits
        {
            .nextInChain = nullptr,
            .limits = limits,
        };

        const WGPUDeviceDescriptor device_descriptor
        {
            .nextInChain = nullptr,
            .label = "Mellohi Device",
            .requiredFeatureCount = 0,
            .requiredFeatures = nullptr,
            .requiredLimits = &required_limits,
            .defaultQueue = WGPUQueueDescriptor
            {
                .nextInChain = nullptr,
                .label = "Mellohi Default Queue",
            },
            .deviceLostCallback = nullptr,
            .deviceLostUserdata = nullptr,
            .deviceLostCallbackInfo = nullptr,
            .uncapturedErrorCallbackInfo = nullptr,
        };

        struct UserData
        {
            WGPUDevice device;
            bool done;
        };
        UserData user_data{};

        auto callback = [](const WGPURequestDeviceStatus status, const WGPUDevice device, const char *message,
                           void *user_data_ptr)
        {
            MH_ASSERT(status == WGPURequestDeviceStatus_Success, "Failed to request WGPU device: {}", message);

            auto *user_data = static_cast<UserData *>(user_data_ptr);
            user_data->device = device;
            user_data->done = true;
        };

        wgpuAdapterRequestDevice(adapter.get_raw_ptr(), &device_descriptor, callback, &user_data);

        MH_ASSERT(user_data.done && user_data.device != nullptr, "Failed to request WGPU device.");

        m_wgpu_device = user_data.device;

        wgpuDeviceSetUncapturedErrorCallback(m_wgpu_device, [](const WGPUErrorType type, const char *message, void *)
        {
            MH_ERROR("WGPU error ({}):\n{}", type, message);
        }, nullptr);
    }

    Device::~Device()
    {
        if (m_wgpu_device != nullptr)
        {
            wgpuDeviceRelease(m_wgpu_device);
            m_wgpu_device = nullptr;
        }
    }

    Device::Device(const Device &other)
    {
        m_adapter = other.m_adapter;

        m_wgpu_device = other.m_wgpu_device;
        if (m_wgpu_device != nullptr)
        {
            wgpuDeviceAddRef(m_wgpu_device);
        }
    }

    Device::Device(Device &&other) noexcept
    {
        std::swap(m_adapter, other.m_adapter);
        std::swap(m_wgpu_device, other.m_wgpu_device);
    }

    auto Device::operator=(const Device &other) -> Device &
    {
        if (this != &other)
        {
            m_adapter = other.m_adapter;

            if (m_wgpu_device != nullptr)
            {
                wgpuDeviceRelease(m_wgpu_device);
            }

            m_wgpu_device = other.m_wgpu_device;
            if (m_wgpu_device != nullptr)
            {
                wgpuDeviceAddRef(m_wgpu_device);
            }
        }

        return *this;
    }

    auto Device::operator=(Device &&other) noexcept -> Device &
    {
        if (this != &other)
        {
            std::swap(m_adapter, other.m_adapter);
            std::swap(m_wgpu_device, other.m_wgpu_device);
        }

        return *this;
    }

    auto Device::tick() const -> void
    {
        wgpuDeviceTick(m_wgpu_device);
    }

    auto Device::get_adapter() const -> Adapter &
    {
        return *m_adapter;
    }

    auto Device::get_limits() const -> WGPULimits
    {
        WGPUSupportedLimits supported_limits{};
        wgpuDeviceGetLimits(m_wgpu_device, &supported_limits);

        return supported_limits.limits;
    }

    auto Device::get_raw_ptr() const -> WGPUDevice
    {
        return m_wgpu_device;
    }
}
