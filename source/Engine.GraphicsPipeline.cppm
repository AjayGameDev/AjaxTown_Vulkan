module;

#include <vulkan/vulkan.h>

export module Engine.GraphicsPipeline;

import Engine.Core;

export class GraphicsPipeline
{
    Context& context;
    VkPipeline pipeline{};
    VkPipelineLayout pipelineLayout{};

public:

    GraphicsPipeline(Context& context,VkPipeline pipeline,VkPipelineLayout pipelineLayout);

    GraphicsPipeline(const GraphicsPipeline&)      = delete;             // No copy through constructor (Vulkan handles are just integers that point to the resources on the gpu)
    GraphicsPipeline& operator=(GraphicsPipeline&) = delete;            //  No copy through = operator  (so you shouldn't copy them otherwise they will point the same resource and lead to double deletion or undefined behaviour)

    GraphicsPipeline(GraphicsPipeline&& other) noexcept;              // can move through constructor (Move is fine as long as you transfer ownership and set current handles to null)
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;  //  can move through = operator  (so they don't try to delete resources on their destructor)

    VkPipeline& GetPipeline(){return pipeline;}
    VkPipelineLayout& GetPipelineLayout(){return pipelineLayout;}

    explicit operator VkPipeline() const { return pipeline; } // Implicitly converting to pipeline for ease of use like vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    ~GraphicsPipeline();
};









































module :private;


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


GraphicsPipeline::~GraphicsPipeline()
{
    vkDeviceWaitIdle(context.device); // to be fixed

    if (pipeline!=nullptr)
        vkDestroyPipeline(context.device,pipeline,nullptr);

    if (pipelineLayout!=nullptr)
        vkDestroyPipelineLayout(context.device,pipelineLayout,nullptr);
}



