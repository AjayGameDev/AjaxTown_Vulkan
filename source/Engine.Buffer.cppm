module;

#include <vulkan/vulkan.h>
#include "dependencies/vk_mem_alloc.h"
#include <memory.h>

export module Engine.Buffer;

import Engine.Context;
import std;

export class Buffer
{

public:

    VkBuffer handle = nullptr;
    uint64_t bufferAddress = 0;
    Context& context;
    VmaAllocation allocation = nullptr;
    VmaAllocationCreateInfo allocationCreateInfo{};
    VkBufferCreateInfo bufferCreateInfo{};



    Buffer(Context& context,const uint64_t& size, const VkBufferUsageFlags& usage,const VmaMemoryUsage& memoryUsageType);
    void CopyData(const void *dataSource, const size_t size);
    VkBuffer& GetHandle() { return handle; }
    uint64_t GetAddress()
    {

        VkBufferDeviceAddressInfo info{};
        info.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = handle;

        bufferAddress = vkGetBufferDeviceAddress(context.device,&info);
        return bufferAddress;
    }
    ~Buffer();

    Buffer(Buffer&& other) noexcept:handle(other.handle),context(other.context),allocation(other.allocation),allocationCreateInfo(other.allocationCreateInfo),bufferCreateInfo(other.bufferCreateInfo),bufferAddress(other.bufferAddress)
    {
        other.handle = VK_NULL_HANDLE;
        other.allocation = VK_NULL_HANDLE;
    }

    Buffer& operator=(Buffer&& other) noexcept
    {
        if (this!=&other)
        {
            if (handle!= VK_NULL_HANDLE)
            {
                vmaDestroyBuffer(context.allocator,handle,allocation);
            }
            handle = other.handle;
            allocation = other.allocation;
            allocationCreateInfo = other.allocationCreateInfo;
            bufferCreateInfo = other.bufferCreateInfo;
            bufferAddress = other.bufferAddress;

            other.handle = VK_NULL_HANDLE;
            other.allocation = VK_NULL_HANDLE;

        }
        return *this;
    }


};

































module :private;


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
    if (handle!=VK_NULL_HANDLE)
    {
        //vkDeviceWaitIdle(context.device); // only use for cleaning resources during closing of the program as it stalls cpu to wait for gpu to go idle before proceeding
        vmaDestroyBuffer(context.allocator,handle,allocation);
    }
}