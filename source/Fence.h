#pragma once
#include "VulkanContext.h"
#include "stdexcept"
#include "string"

class Fence 
{
    public:
            explicit Fence(VulkanContext& context,bool isSignaled = true);
            ~Fence();

            void Create(bool isSignaled);

            Fence(const Fence&) = delete;
            Fence& operator=(const Fence&) = delete;

            inline VkFence GetHandle()
            {
                return handle;
            }

            inline void Wait(uint64_t timeout = UINT64_MAX)
            {
                vkWaitForFences(context.device,1,&handle,VK_TRUE,timeout);
            }

            inline void Reset()
            {
                vkResetFences(context.device,1,&handle);
            }

            inline void WaitAndReset(uint64_t timeout = UINT64_MAX)
            {
                vkWaitForFences(context.device,1,&handle,VK_TRUE,timeout);
                vkResetFences(context.device,1,&handle);
            }

            inline bool IsSignaled()
            {
                VkResult result = vkGetFenceStatus(context.device,handle);

                if (result == VK_SUCCESS)
                    return true;

                if (result == VK_NOT_READY)
                    return false;

                throw std::runtime_error("\nError occured when trying to check fence status" + std::to_string(result) );
            }

    private:
            VkFence handle = VK_NULL_HANDLE;
            VulkanContext& context;
};
