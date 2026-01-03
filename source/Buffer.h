#pragma once



class Context;

class Buffer 
{

  public:
          Buffer(Context& context,const uint64_t& size, const VkBufferUsageFlags& usage,const VmaMemoryUsage& memoryUsageType);
          void CopyData(const void *dataSource, const size_t size);
          VkBuffer& GetHandle() { return handle; }
         ~Buffer();


          VkBuffer handle = nullptr;
          Context& context;
          VmaAllocation allocation = nullptr;
          VmaAllocationCreateInfo allocationCreateInfo{};
          VkBufferCreateInfo bufferCreateInfo{};

};
