#include "Buffer.h"
#include "string"

Buffer::Buffer(const VmaAllocator &allocator, const float &size,
               const VkBufferUsageFlags &usage,
               const VmaMemoryUsage &memoryUsageType)
    : allocator(allocator)
{

  buffer = nullptr;

  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = usage;

  allocationCreateInfo.usage = memoryUsageType;

  if (vmaCreateBuffer(this->allocator, &bufferCreateInfo, &allocationCreateInfo,
                      &buffer, &allocation, nullptr) != VK_SUCCESS)
  {
    throw std::runtime_error("\nCan't create VMA buffer!");
  }
}

void Buffer::CopyData(const void *dataSource, const size_t size)
{
  void* bufferDataPointer;
  vmaMapMemory(allocator,allocation,&bufferDataPointer);
  memcpy(bufferDataPointer,&dataSource,size);
  vmaUnmapMemory(allocator,allocation);
}

Buffer::~Buffer()
{

  vmaDestroyBuffer(allocator,buffer,allocation);

}