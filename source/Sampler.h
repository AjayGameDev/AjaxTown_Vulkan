#pragma once



class Sampler 
{
    Context& context;
    VkSampler handle = VK_NULL_HANDLE;
    VkSamplerCreateInfo samplerCreateInfo{};

public:
    Sampler(Context& context);
    VkSampler& GetHandle()
    {
        return handle;
    }
};
