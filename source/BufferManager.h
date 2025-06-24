#pragma once

#include "Buffer.h"
#include "Dependencies/vk_mem_alloc.h"
#include "stdexcept"

class BufferManager 
{

  public:
         VmaAllocator allocator = nullptr;
         BufferManager(VkDevice device,VkPhysicalDevice physicalDevice,VkInstance instance,int32_t queueFamilyIndex,VkQueue graphicsQueue);
         Buffer CreateBuffer(size_t size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsageType);
         void CopyBuffer(Buffer* sourceBuffer,Buffer* destinationBuffer);

        ~BufferManager();

  private:
          VkDevice device;
          VkPhysicalDevice physicalDevice;
          VkInstance instace;
          int32_t queueFamilyIndex;
          VkQueue graphicsQueue;
          VkCommandBuffer BeginOneTimeCommandBuffer( VkCommandPool commandPool);
          void EndOneTimeCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkDevice device, VkCommandPool commandPool);

};
