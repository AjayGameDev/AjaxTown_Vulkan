#include "Fence.h"


Fence::Fence(VulkanContext &context, bool isSignaled) : context(context)
{
    Create(isSignaled);
}

void Fence::Create(bool isSignaled)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = isSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkResult result = vkCreateFence(context.device,&fenceCreateInfo,nullptr,&handle);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("\nCan't create fence" + std::to_string(result));
    }
}

Fence::~Fence()
{
    if (handle  !=  VK_NULL_HANDLE)
    {
        vkDestroyFence(context.device,handle,nullptr);
    }
}



