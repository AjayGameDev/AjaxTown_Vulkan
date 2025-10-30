#include "Semaphore.h"
#include "stdexcept"
#include "string"

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

