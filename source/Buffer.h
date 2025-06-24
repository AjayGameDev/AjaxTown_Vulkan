#pragma once

#include "Dependencies/vk_mem_alloc.h"
#include "stdexcept"

class Buffer 
{

  public:
          Buffer(const VmaAllocator& allocator,const float& size, const VkBufferUsageFlags& usage,const VmaMemoryUsage& memoryUsageType);
          void CopyData(const void *dataSource, const size_t size);
         ~Buffer();


          VkBuffer buffer = nullptr;
          VmaAllocation allocation = nullptr;
          VmaAllocator allocator  = nullptr;
          VmaAllocationCreateInfo allocationCreateInfo{};
          VkBufferCreateInfo bufferCreateInfo{};

};
