#pragma once
#include <ComputePipeline.h>
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <vector>

#include "Context.h"

class ComputePipelineBuilder
{
    VkPipeline pipeline = VK_NULL_HANDLE;               // use VK_NULL_HANDLE for non dispatchable handles
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo    shaderStageInfo{}; // Important to zero intialize structs
    std::vector<VkPushConstantRange>   pushConstants;
    std::vector<VkDescriptorSetLayout> descriptorSetsLayouts;

public:

    ComputePipelineBuilder& AddPushConstant(VkPushConstantRange pushConstant)
    {
        pushConstants.push_back(pushConstant);
        return *this;
    }

    ComputePipelineBuilder& AddDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
    {
        descriptorSetsLayouts.push_back(descriptorSetLayout);
        return *this;
    }

    ComputePipelineBuilder& SetComputeShader(VkShaderModule computeShader)
    {
        shaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageInfo.pName  = "main";
        shaderStageInfo.module = computeShader;

        return *this;
    }

    ComputePipeline Build(Context& context)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType                    =   VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pushConstantRangeCount   =   static_cast<uint32_t>(pushConstants.size());
        pipelineLayoutCreateInfo.pPushConstantRanges      =   pushConstants.data();
        pipelineLayoutCreateInfo.setLayoutCount           =   static_cast<uint32_t>(descriptorSetsLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts              =   descriptorSetsLayouts.data();

        if (vkCreatePipelineLayout(context.device,&pipelineLayoutCreateInfo,nullptr,&pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("\nCan't create compute pipeline layout");

        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.layout = pipelineLayout;
        computePipelineCreateInfo.stage  = shaderStageInfo;

        if (vkCreateComputePipelines(context.device,nullptr,1,&computePipelineCreateInfo,nullptr,&pipeline) != VK_SUCCESS)
            throw std::runtime_error("\nCan't create compute pipeline! ");

        ComputePipeline result = ComputePipeline(context,pipeline,pipelineLayout);

        return result;

    }
};
