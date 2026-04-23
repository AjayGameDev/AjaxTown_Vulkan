#pragma once




class Buffer
{
    uint64_t bufferAddress = 0;

public:
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


    VkBuffer handle = nullptr;
    Context& context;
    VmaAllocation allocation = nullptr;
    VmaAllocationCreateInfo allocationCreateInfo{};
    VkBufferCreateInfo bufferCreateInfo{};
};