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
