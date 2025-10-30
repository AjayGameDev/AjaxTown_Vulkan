#pragma once
#include "Context.h"
#include <vulkan/vulkan.h>

class GraphicsPipeline 
{
    Context& context;
    VkPipeline pipeline = nullptr;
    VkPipelineLayout pipelineLayout = nullptr;

public:

    GraphicsPipeline(Context& context,VkPipeline pipeline,VkPipelineLayout pipelineLayout);

    GraphicsPipeline(const GraphicsPipeline&)      = delete;             // No copy through constructor (Vulkan handles are just integers that point to the resources on the gpu)
    GraphicsPipeline& operator=(GraphicsPipeline&) = delete;            //  No copy through = operator  (so you shouldn't copy them otherwise they will point the same resource and lead to double deletion or undefined behaviour)

    GraphicsPipeline(GraphicsPipeline&& other) noexcept;              // can move through constructor (Move is fine as long as you transfer ownership and set current handles to null)
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;  //  can move through = operator  (so they don't try to delete resources on their destructor)

    VkPipeline& GetPipeline();
    VkPipelineLayout& GetPipelineLayout();

    explicit operator VkPipeline() const { return pipeline; } // Implicitly converting to pipeline for ease of use like vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    ~GraphicsPipeline();
};
