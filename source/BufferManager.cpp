#include "BufferManager.h"

BufferManager::BufferManager(Context& context):context(context)
{

}

Buffer BufferManager::CreateBuffer(size_t size, VkBufferUsageFlags usage,
                                   VmaMemoryUsage memoryUsageType)
{

  Buffer buffer(context, size, usage, memoryUsageType);
  return buffer;
}

//----------------------------------------------------------------------------------------------------------   Copy data from one bufer to other buffer
void BufferManager::CopyBuffer(Buffer *sourceBuffer, Buffer *destinationBuffer)
{

  VkCommandPool tempCommandPool = nullptr;

  VkCommandPoolCreateInfo tempCommandPoolCreateInfo{};

  tempCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  tempCommandPoolCreateInfo.queueFamilyIndex = context.transferFamilyIndex;
  tempCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // This command pool is for one time command buffer

  VkResult result = vkCreateCommandPool(context.device,&tempCommandPoolCreateInfo,nullptr,&tempCommandPool);
  if (result != VK_SUCCESS)
    throw std::runtime_error("\nCan't create command pool " + result);

  const VkCommandBuffer copyCommand =  BeginOneTimeCommandBuffer(tempCommandPool);

  VkBufferCopy bufferCopyRegion{};
  bufferCopyRegion.size = sourceBuffer->bufferCreateInfo.size;
  vkCmdCopyBuffer(copyCommand,sourceBuffer->handle,destinationBuffer->handle,1,&bufferCopyRegion);
  EndOneTimeCommandBuffer(copyCommand,context.transferQueue,context.device,tempCommandPool);

}

//---------------------------------------------------------------------------------------------------------- Helper Functions

VkCommandBuffer BufferManager::BeginOneTimeCommandBuffer(VkCommandPool commandPool)
{
  VkCommandBuffer commandBuffer = nullptr;
  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandBufferCount = 1;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  vkAllocateCommandBuffers(context.device,&commandBufferAllocateInfo,&commandBuffer);

  VkCommandBufferBeginInfo commandBufferBeginInfo{};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer,&commandBufferBeginInfo);

  return commandBuffer;

}

void BufferManager::EndOneTimeCommandBuffer( VkCommandBuffer commandBuffer, VkQueue queue, VkDevice device, VkCommandPool commandPool)
{
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(queue,1,&submitInfo,VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);                                       // Check later

  vkFreeCommandBuffers(device,commandPool,1,&commandBuffer);
  vkDestroyCommandPool(device,commandPool,nullptr);

}

BufferManager::~BufferManager()
{

}