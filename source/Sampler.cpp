#include "Sampler.h"


Sampler::Sampler(Context& context):context(context)
{
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

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
