#include "mellohi/graphics/wgpu/sampler.hpp"

namespace mellohi::wgpu
{
    Sampler::Sampler(const Device &device, const string &label, const bool linearly_filter)
    {
        const WGPUSamplerDescriptor sampler_descriptor
        {
            .nextInChain = nullptr,
            .label = label.c_str(),
            .addressModeU = WGPUAddressMode_ClampToEdge,
            .addressModeV = WGPUAddressMode_ClampToEdge,
            .addressModeW = WGPUAddressMode_ClampToEdge,
            .magFilter = linearly_filter ? WGPUFilterMode_Linear : WGPUFilterMode_Nearest,
            .minFilter = linearly_filter ? WGPUFilterMode_Linear : WGPUFilterMode_Nearest,
            .mipmapFilter = WGPUMipmapFilterMode_Linear,
            .lodMinClamp = 0.0f,
            .lodMaxClamp = 8.0f,
            .compare = WGPUCompareFunction_Undefined,
            .maxAnisotropy = 1,
        };

        m_wgpu_sampler = wgpuDeviceCreateSampler(device.get_raw_ptr(), &sampler_descriptor);
    }

    Sampler::~Sampler()
    {
        if (m_wgpu_sampler != nullptr)
        {
            wgpuSamplerRelease(m_wgpu_sampler);
            m_wgpu_sampler = nullptr;
        }
    }

    Sampler::Sampler(const Sampler &other)
    {
        m_wgpu_sampler = other.m_wgpu_sampler;
        if (m_wgpu_sampler != nullptr)
        {
            wgpuSamplerAddRef(m_wgpu_sampler);
        }
    }

    Sampler::Sampler(Sampler &&other) noexcept
    {
        std::swap(m_wgpu_sampler, other.m_wgpu_sampler);
    }

    auto Sampler::operator=(const Sampler &other) -> Sampler &
    {
        if (this != &other)
        {
            if (m_wgpu_sampler != nullptr)
            {
                wgpuSamplerRelease(m_wgpu_sampler);
            }

            m_wgpu_sampler = other.m_wgpu_sampler;
            if (m_wgpu_sampler != nullptr)
            {
                wgpuSamplerAddRef(m_wgpu_sampler);
            }
        }

        return *this;
    }

    auto Sampler::operator=(Sampler &&other) noexcept -> Sampler &
    {
        if (this != &other)
        {
            std::swap(m_wgpu_sampler, other.m_wgpu_sampler);
        }

        return *this;
    }

    auto Sampler::get_raw_ptr() const -> WGPUSampler
    {
        return m_wgpu_sampler;
    }
}
