#include "Buffer.h"

Buffer::Buffer(Context& context, const uint64_t &size, const VkBufferUsageFlags &usage, const VmaMemoryUsage &memoryUsageType) : context(context)
{

  handle = nullptr;

  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size  = size;
  bufferCreateInfo.usage = usage;

  allocationCreateInfo.usage = memoryUsageType;

  if (vmaCreateBuffer(context.allocator, &bufferCreateInfo, &allocationCreateInfo, &handle, &allocation, nullptr) != VK_SUCCESS)
  {
    throw std::runtime_error("\nCan't create VMA buffer!");
  }
}

void Buffer::CopyData(const void *dataSource, const size_t size)
{
  void* bufferDataPointer;
  vmaMapMemory(context.allocator,allocation,&bufferDataPointer);
  memcpy(bufferDataPointer,dataSource,size);
  vmaUnmapMemory(context.allocator,allocation);
}

Buffer::~Buffer()
{
  vkDeviceWaitIdle(context.device); // only use for cleaning resources during closing of the program as it stalls cpu to wait for gpu to go idle before proceeding
  vmaDestroyBuffer(context.allocator,handle,allocation);
}