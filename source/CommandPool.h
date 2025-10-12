#pragma once
#include <vector>
#include "VulkanContext.h"

class CommandBuffer;

class CommandPool 
{
    private:
            VkCommandPool handle = VK_NULL_HANDLE;
            uint32_t queueFamilyIndex = 0;
            std::vector<VkCommandBuffer> commandBuffers;
            VulkanContext& context;

    public:
            explicit CommandPool(VulkanContext& context);
            ~CommandPool();
            void Create(uint32_t queueFamiliyIndex = 0,VkCommandPoolCreateFlags flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
            void Destroy();
            void Reset();
            CommandBuffer Allocate(VkCommandBufferLevel commandBufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
};
