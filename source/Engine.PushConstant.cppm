module;
#include <vulkan/vulkan.h>

export module Engine.PushConstant;





export class PushConstant
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
