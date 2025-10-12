#pragma once

#include "vk_mem_alloc.h"
#include "stdexcept"

class Buffer 
{

  public:
          Buffer(VkDevice device,const VmaAllocator& allocator,const uint64_t& size, const VkBufferUsageFlags& usage,const VmaMemoryUsage& memoryUsageType);
          void CopyData(const void *dataSource, const size_t size);
         ~Buffer();


          VkBuffer buffer = nullptr;
          VkDevice device;
          VmaAllocation allocation = nullptr;
          VmaAllocator  allocator  = nullptr;
          VmaAllocationCreateInfo allocationCreateInfo{};
          VkBufferCreateInfo bufferCreateInfo{};

};
