#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline(VulkanContext &context, VkPipeline pipeline, VkPipelineLayout pipelineLayout) : context(context),pipeline(pipeline),pipelineLayout(pipelineLayout) {}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept :  context(other.context), pipeline(other.pipeline), pipelineLayout(other.pipelineLayout)
{
    other.pipeline = nullptr;
    other.pipelineLayout = nullptr;
}

GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept :  context(other.context)
{
    if (this!=&other)
    {
        if (pipeline!=nullptr)
            vkDestroyPipeline(context.device,pipeline,nullptr);

        if (pipelineLayout!=nullptr)
            vkDestroyPipelineLayout(context.device,pipelineLayout,nullptr);
    }

    pipeline       = other.pipeline;
    pipelineLayout = other.pipelineLayout;

    other.pipeline       = nullptr;
    other.pipelineLayout = nullptr;

    return *this;
}

inline VkPipeline GraphicsPipeline::GetPipeline() const
{
    return pipeline;
}

VkPipelineLayout GraphicsPipeline::GetPipelineLayout() const
{
    return pipelineLayout;
}

GraphicsPipeline::~GraphicsPipeline()
{
    if (pipeline!=nullptr)
        vkDestroyPipeline(context.device,pipeline,nullptr);

    if (pipelineLayout!=nullptr)
        vkDestroyPipelineLayout(context.device,pipelineLayout,nullptr);
}



