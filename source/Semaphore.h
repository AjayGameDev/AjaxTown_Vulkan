#pragma once
#include "VulkanContext.h"


class Semaphore 
{
    private:
            VkSemaphore handle = VK_NULL_HANDLE;
            VulkanContext& context;

        public:
            Semaphore(VulkanContext& context);
            ~Semaphore();
            inline VkSemaphore GetHandle() { return handle; } // functions defined inside the class are implicitly inline

};
