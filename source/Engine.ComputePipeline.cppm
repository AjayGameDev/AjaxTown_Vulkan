module;

#include <vulkan/vulkan.h>

export module Engine.ComputePipeline;

import Engine.Core;

export class ComputePipeline
{
    Context& context;
    VkPipeline pipeline{};
    VkPipelineLayout pipelineLayout{};

public:

    ComputePipeline(Context& context,VkPipeline pipeline,VkPipelineLayout pipelineLayout);

    ComputePipeline(const ComputePipeline&)      = delete;             // No copy through constructor (Vulkan handles are just integers that point to the resources on the gpu)
    ComputePipeline& operator=(ComputePipeline&) = delete;            //  No copy through = operator  (so you shouldn't copy them otherwise they will point the same resource and lead to double deletion or undefined behaviour)

    ComputePipeline(ComputePipeline&& other) noexcept;              // can move through constructor (Move is fine as long as you transfer ownership and set current handles to null)
    ComputePipeline& operator=(ComputePipeline&& other) noexcept;  //  can move through = operator  (so they don't try to delete resources on their destructor)

    VkPipeline& GetPipeline() {return pipeline;}
    VkPipelineLayout& GetPipelineLayout() {return pipelineLayout;}

    explicit operator VkPipeline() const { return pipeline; } // Implicitly converting to pipeline for ease of use like vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    ~ComputePipeline();
};













































module :private;



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
