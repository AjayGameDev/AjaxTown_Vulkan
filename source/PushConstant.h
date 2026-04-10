#pragma once
#include <vulkan/vulkan_core.h>


class PushConstant 
{
    VkPushConstantRange pushConstantRange{};

public:

    PushConstant() = delete;

    PushConstant(VkShaderStageFlags shaderStageFlags,uint32_t offset,uint32_t size)
    {
        pushConstantRange.stageFlags = shaderStageFlags;
        pushConstantRange.offset = offset;
        pushConstantRange.size = size;
    }

    VkPushConstantRange& GetHandle()
    {
        return pushConstantRange;
    }

};
