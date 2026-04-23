#pragma once

#include "Buffer.h"


class BufferManager
{

public:
    explicit BufferManager(Context& context);
    Buffer CreateBuffer(size_t size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsageType);
    void CopyBuffer(Buffer* sourceBuffer,Buffer* destinationBuffer);

    ~BufferManager();

private:
    Context& context;

    VkCommandBuffer BeginOneTimeCommandBuffer( VkCommandPool commandPool);
    void EndOneTimeCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkDevice device, VkCommandPool commandPool);

};