#include "ComputePipeline.h"

ComputePipeline::ComputePipeline(Context &context, VkPipeline pipeline, VkPipelineLayout pipelineLayout):context(context),pipeline(pipeline),pipelineLayout(pipelineLayout) {}

ComputePipeline::ComputePipeline(ComputePipeline &&other) noexcept : context(other.context),pipeline(other.pipeline),pipelineLayout(other.pipelineLayout)
{
    other.pipeline = nullptr;
    other.pipelineLayout = nullptr;
}

ComputePipeline& ComputePipeline::operator=(ComputePipeline &&other) noexcept
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


ComputePipeline::~ComputePipeline()
{
    vkDeviceWaitIdle(context.device); // to be fixed

    if (pipeline!=nullptr)
        vkDestroyPipeline(context.device,pipeline,nullptr);

    if (pipelineLayout!=nullptr)
        vkDestroyPipelineLayout(context.device,pipelineLayout,nullptr);
}
