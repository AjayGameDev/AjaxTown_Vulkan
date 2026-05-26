module;

#include <vulkan/vulkan.h>
#include "spdlog/spdlog.h"

export module Engine.Sampler;

import Engine.Core;

export class Sampler
{
    Context& context;
    VkSampler handle = VK_NULL_HANDLE;
    VkSamplerCreateInfo samplerCreateInfo{};

public:

    explicit Sampler(Context& context);
    VkSampler& GetHandle()
    {
        return handle;
    }
    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;

    Sampler(Sampler&& other) noexcept:context(other.context),handle(other.handle)
    {
        other.handle = VK_NULL_HANDLE;
    }

    Sampler& operator=(Sampler&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            handle = other.handle;
            other.handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    void Destroy();
    ~Sampler();

};
















module :private; // this will allow us to not trigger recompilation if we change anything in implmentation


Sampler::Sampler(Context& context):context(context)
{
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.minLod     = 0;
    samplerCreateInfo.maxLod     = VK_LOD_CLAMP_NONE; // 1000 unlocks all mip levels
    samplerCreateInfo.addressModeU = samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //samplerCreateInfo.addressModeU = samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    // add in future based on physical properties and options in menu
    //samplerCreateInfo.anisotropyEnable = true;
    //samplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;

    if (vkCreateSampler(context.device,&samplerCreateInfo,nullptr,&handle) != VK_SUCCESS)
    {
        spdlog::error("Can't create sampler!");
        throw std::runtime_error("Can't create sampler!");
    }
}

void Sampler::Destroy()
{
    if (handle != VK_NULL_HANDLE)
    {
        vkDestroySampler(context.device,handle,nullptr);
        handle = VK_NULL_HANDLE;
    }
}

Sampler::~Sampler()
{
    Destroy();
}
