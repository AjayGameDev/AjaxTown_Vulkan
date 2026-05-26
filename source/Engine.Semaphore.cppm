module;

#include <vulkan/vulkan.h>

export module Engine.Semaphore;

import Engine.Core;

export class Semaphore
{

private:
    VkSemaphore handle = VK_NULL_HANDLE;
    Context& context;

public:
    explicit Semaphore(Context& context);
    ~Semaphore();

    // Delete copy constructors
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    // movable
    Semaphore(Semaphore&& other) noexcept : handle(other.handle), context(other.context)
    {
        other.handle = VK_NULL_HANDLE;
    }
    Semaphore& operator=(Semaphore&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            handle = other.handle;
            other.handle = VK_NULL_HANDLE;
        }
        return *this;
    }

    void Create();
    void Destroy();
    VkSemaphore& GetHandle() { return handle; } // functions defined inside the class are implicitly inline

};











module :private;

Semaphore::Semaphore(Context& context) : context(context)
{
    Create();
}

void Semaphore::Create()
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(context.device,&semaphoreCreateInfo,nullptr,&handle);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("\nCan't create semaphore " + std::to_string(result));
    }
}

Semaphore::~Semaphore()
{
    Destroy();
}


void Semaphore::Destroy()
{
    if (handle != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(context.device,handle,nullptr);
        handle = VK_NULL_HANDLE;
    }
}

