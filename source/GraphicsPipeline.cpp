#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline(Context &context, VkPipeline pipeline, VkPipelineLayout pipelineLayout) : context(context),pipeline(pipeline),pipelineLayout(pipelineLayout) {}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept :  context(other.context), pipeline(other.pipeline), pipelineLayout(other.pipelineLayout)
{
    other.pipeline = nullptr;
    other.pipelineLayout = nullptr;
}

GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept
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

VkPipeline& GraphicsPipeline::GetPipeline()
{
    return pipeline;
}

VkPipelineLayout& GraphicsPipeline::GetPipelineLayout()
{
    return pipelineLayout;
}

GraphicsPipeline::~GraphicsPipeline()
{
    vkDeviceWaitIdle(context.device); // to be fixed

    if (pipeline!=nullptr)
        vkDestroyPipeline(context.device,pipeline,nullptr);

    if (pipelineLayout!=nullptr)
        vkDestroyPipelineLayout(context.device,pipelineLayout,nullptr);
}



