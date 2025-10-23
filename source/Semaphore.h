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

            // Delete copy constructors
            Semaphore(const Semaphore&) = delete;
            Semaphore& operator=(const Semaphore&) = delete;

            void Create();
            inline  VkSemaphore GetHandle() const { return handle; } // functions defined inside the class are implicitly inline

};
