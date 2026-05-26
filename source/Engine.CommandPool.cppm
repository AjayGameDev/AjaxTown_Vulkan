module;

#include <vulkan/vulkan.h>


export module Engine.CommandPool;

import Engine.Core;
import Engine.CommandBuffer;

export class CommandPool
{

private:
    VkCommandPool handle = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex = 0;
    std::vector<VkCommandBuffer> commandBuffers;
    Context& context;

public:
    explicit CommandPool(Context& context);
    ~CommandPool();
    void Create(uint32_t queueFamiliyIndex = 0,VkCommandPoolCreateFlags flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    void Destroy();
    void Reset();
    CommandBuffer Allocate(VkCommandBufferLevel commandBufferLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
};















































module :private;


CommandPool::CommandPool(Context& context) : context(context)
{

}

CommandPool::~CommandPool()
{
    Destroy();
}

void CommandPool::Create(uint32_t queueFamiliyIndex,VkCommandPoolCreateFlags flag)
{
    this->queueFamilyIndex = queueFamiliyIndex;
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};

    commandPoolCreateInfo.sType             =  VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex  =  queueFamilyIndex;
    commandPoolCreateInfo.flags             =  flag;

    VkResult result = vkCreateCommandPool(context.device,&commandPoolCreateInfo,nullptr,&handle);

    if (result!=VK_SUCCESS)
        throw std::runtime_error("Can't create command pool! " + std::to_string(result));
}

void CommandPool::Destroy()
{
    if (handle != VK_NULL_HANDLE)
    {
        if (!commandBuffers.empty())
        {
            vkFreeCommandBuffers(context.device,handle,commandBuffers.size(),commandBuffers.data());
            commandBuffers.clear();
        }
        vkDestroyCommandPool(context.device,handle,nullptr);
        handle = VK_NULL_HANDLE;
    }
}

void CommandPool::Reset()
{
    vkResetCommandPool(context.device,handle,0);
}

CommandBuffer CommandPool::Allocate(VkCommandBufferLevel commandBufferLevel)
{
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};

    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.commandPool = handle;
    commandBufferAllocateInfo.level = commandBufferLevel;

    vkAllocateCommandBuffers(context.device,&commandBufferAllocateInfo,&commandBuffer);
    commandBuffers.push_back(commandBuffer);

    return CommandBuffer(commandBuffer);
}



