#pragma once

#include "vma/vk_mem_alloc.h"
#include "Context.h"
#include "stdexcept"

class Buffer 
{

  public:
          Buffer(Context& context,const uint64_t& size, const VkBufferUsageFlags& usage,const VmaMemoryUsage& memoryUsageType);
          void CopyData(const void *dataSource, const size_t size);
         ~Buffer();


          VkBuffer buffer = nullptr;
          Context& context;
          VmaAllocation allocation = nullptr;
          VmaAllocationCreateInfo allocationCreateInfo{};
          VkBufferCreateInfo bufferCreateInfo{};

};
