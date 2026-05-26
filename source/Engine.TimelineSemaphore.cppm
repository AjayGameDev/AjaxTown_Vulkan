module;

#include <vulkan/vulkan.h>

export module Engine.TimelineSemaphore;

import Engine.Core;

export class TimelineSemaphore
{


    VkSemaphore handle = VK_NULL_HANDLE;
    Context& context;

public:
    explicit TimelineSemaphore(Context& context);
    ~TimelineSemaphore();

    void Create();
    void Destroy();

    VkSemaphore& GetHandle() { return handle; }


    // Delete copy constructors
    TimelineSemaphore(const TimelineSemaphore&) = delete;
    TimelineSemaphore& operator=(const TimelineSemaphore&) = delete;

    // movable
    TimelineSemaphore(TimelineSemaphore&& other) noexcept : handle(other.handle), context(other.context)
    {
        other.handle = VK_NULL_HANDLE;
    }
    TimelineSemaphore& operator=(TimelineSemaphore&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            handle = other.handle;
            other.handle = VK_NULL_HANDLE;
        }
        return *this;
    }
};

















module :private;


TimelineSemaphore::TimelineSemaphore(Context& context) : context(context)
{
    Create();
}


void TimelineSemaphore::Create()
{

    VkSemaphoreTypeCreateInfo semaphoreTypeInfo{};
    semaphoreTypeInfo.sType          =  VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphoreTypeInfo.initialValue   =  0;
    semaphoreTypeInfo.semaphoreType  =  VK_SEMAPHORE_TYPE_TIMELINE;

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = &semaphoreTypeInfo;

    VkResult result = vkCreateSemaphore(context.device,&semaphoreCreateInfo,nullptr,&handle);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("\nCan't create semaphore " + std::to_string(result));
    }
}


void TimelineSemaphore::Destroy()
{
    if (handle!= VK_NULL_HANDLE)
        vkDestroySemaphore(context.device,handle,nullptr);
    handle = VK_NULL_HANDLE;
}


TimelineSemaphore::~TimelineSemaphore()
{
    Destroy();
}