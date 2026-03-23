#include "Descriptor.h"
//#include "spirv_glsl.hpp"
#include "array"
#include "Buffer.h"
#include "Framebuffer.h"


Descriptor::Descriptor(Context &context) : context(context) {}

Descriptor::~Descriptor()
{
    // This should be destroyed after pipeline and pipeline layout
    vkDeviceWaitIdle(context.device); // to be fixed
    vkDestroyDescriptorSetLayout(context.device,globalSetLayout,nullptr);
    vkDestroyDescriptorSetLayout(context.device,computeSetLayout,nullptr);
    vkDestroyDescriptorPool(context.device,descriptorPool,nullptr);
}


void Descriptor::CreateMemoryPool()
{
    // Descriptor pool
    std::array<VkDescriptorPoolSize,6> poolSizes;

    poolSizes[0] = {VK_DESCRIPTOR_TYPE_SAMPLER,20};
    poolSizes[1] = {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,20};
    poolSizes[2] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,20};
    poolSizes[3] = {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,20};
    poolSizes[4] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,20};
    poolSizes[5] = {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,20};


    VkDescriptorPoolCreateInfo poolCreateInfo{};

    poolCreateInfo.sType          =  VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets        =  16;
    poolCreateInfo.poolSizeCount  =  poolSizes.size();
    poolCreateInfo.pPoolSizes     =  poolSizes.data();


    vkCreateDescriptorPool(context.device,&poolCreateInfo,nullptr,&descriptorPool);
}

void Descriptor::CreateGlobalSetLayout()
{
    std::array<VkDescriptorSetLayoutBinding,1> bindings;

    //bindings[0] = {0,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  1,VK_SHADER_STAGE_VERTEX_BIT};
    bindings[0] = {0,VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,1,VK_SHADER_STAGE_FRAGMENT_BIT};

    // Descriptor set layout
    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};

    layoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = bindings.size();
    layoutCreateInfo.pBindings    = bindings.data();


    vkCreateDescriptorSetLayout(context.device,&layoutCreateInfo,nullptr,&globalSetLayout);
}

void Descriptor::AllocateGlobalSet()
{
    // Allocating Descriptor sets
    VkDescriptorSetAllocateInfo allocationInfo{};

    allocationInfo.sType               =  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocationInfo.descriptorPool      =  descriptorPool;
    allocationInfo.descriptorSetCount  =  1;
    allocationInfo.pSetLayouts         =  &globalSetLayout;


    vkAllocateDescriptorSets(context.device,&allocationInfo,&globalSet);
}


void Descriptor::UpdateGlobalSet(Framebuffer& frameBuffer)
{
    //VkDescriptorBufferInfo vertexBufferInfo{vertexBuffer.GetHandle(),0,VK_WHOLE_SIZE};
    VkDescriptorImageInfo  hdrImageInfo{nullptr,frameBuffer.resolvedImage.imageView,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    std::array<VkWriteDescriptorSet,1> writes{}; // use {} to get default values otherwise uninitialized memory will cause crash

    //writes[0].sType             =   VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //writes[0].dstSet            =   globalSet;
    //writes[0].dstBinding        =   0;
    //writes[0].descriptorType    =   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //writes[0].descriptorCount   =   1;
    //writes[0].pBufferInfo       =   &vertexBufferInfo;

    writes[0].sType             =  VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet            =  globalSet;
    writes[0].dstBinding        =  0;
    writes[0].descriptorType    =  VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    writes[0].descriptorCount   =  1;
    writes[0].pImageInfo        =  &hdrImageInfo;

    vkUpdateDescriptorSets(context.device,writes.size(),writes.data(),0,nullptr);
}



void Descriptor::CreateComputeSetLayout()
{
    std::array<VkDescriptorSetLayoutBinding,2> bindings;

    bindings[0] = {0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,  1,VK_SHADER_STAGE_COMPUTE_BIT};
    bindings[1] = {1,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,  1,VK_SHADER_STAGE_COMPUTE_BIT};

    // Descriptor set layout
    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};

    layoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = bindings.size();
    layoutCreateInfo.pBindings    = bindings.data();

    vkCreateDescriptorSetLayout(context.device,&layoutCreateInfo,nullptr,&computeSetLayout);
}



void Descriptor::AllocateComputeSet()
{
    // Allocating Descriptor sets
    VkDescriptorSetAllocateInfo allocationInfo{};

    allocationInfo.sType               =  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocationInfo.descriptorPool      =  descriptorPool;
    allocationInfo.descriptorSetCount  =  1;
    allocationInfo.pSetLayouts         =  &computeSetLayout;


    vkAllocateDescriptorSets(context.device,&allocationInfo,&computeSet);
}

void Descriptor::UpdateComputeSet(Buffer& indirectBuffer, Buffer& countBuffer)
{
    VkDescriptorBufferInfo indirectBufferInfo { indirectBuffer.GetHandle(), 0, VK_WHOLE_SIZE };
    VkDescriptorBufferInfo countBufferInfo    { countBuffer.GetHandle(),    0, VK_WHOLE_SIZE };

    std::array<VkWriteDescriptorSet,2> writes{}; // use {} to get default values otherwise uninitialized memory will cause crash

    writes[0].sType             =   VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet            =   computeSet;
    writes[0].dstBinding        =   0;
    writes[0].descriptorType    =   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[0].descriptorCount   =   1;
    writes[0].pBufferInfo       =   &indirectBufferInfo;

    writes[1].sType             =   VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet            =   computeSet;
    writes[1].dstBinding        =   1;
    writes[1].descriptorType    =   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].descriptorCount   =   1;
    writes[1].pBufferInfo       =   &countBufferInfo;

    vkUpdateDescriptorSets(context.device,writes.size(),writes.data(),0,nullptr);
}



void Descriptor::ReflectBindings(const std::vector<uint32_t>& spirvCode,ShaderType type)
{
    /*
    spirv_cross::CompilerGLSL compiler(spirvCode);
    auto resources = compiler.get_shader_resources();

    for (auto& resource: resources.sampled_images)
    {
        BindingInfo binding;

        binding.name             =  compiler.get_name(resource.id);
        binding.set              =  compiler.get_decoration(resource.id,spv::DecorationDescriptorSet);
        binding.binding          =  compiler.get_decoration(resource.id,spv::DecorationBinding);
        binding.type             =  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.shaderStage      =  ShaderTypeToStage(type);
        binding.updateFrequency  =  SetToFrequency(binding.set);

        bindings.push_back(binding);

    }

    for (auto& resource: resources.uniform_buffers)
    {
        BindingInfo binding;

        binding.name             =  compiler.get_name(resource.id);
        binding.set              =  compiler.get_decoration(resource.id,spv::DecorationDescriptorSet);
        binding.binding          =  compiler.get_decoration(resource.id,spv::DecorationBinding);
        binding.shaderStage      =  ShaderTypeToStage(type);
        binding.updateFrequency  =  SetToFrequency(binding.set);

        if (compiler.has_decoration(resource.id,spv::DecorationBufferBlock))
            binding.type             =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        else
            binding.type             =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        bindings.push_back(binding);
    }

    for (auto& resource: resources.storage_buffers)
    {
        BindingInfo binding;

        binding.name             =  compiler.get_name(resource.id);
        binding.set              =  compiler.get_decoration(resource.id,spv::DecorationDescriptorSet);
        binding.binding          =  compiler.get_decoration(resource.id,spv::DecorationBinding);
        binding.shaderStage      =  ShaderTypeToStage(type);
        binding.updateFrequency  =  SetToFrequency(binding.set);

        if (compiler.has_decoration(resource.id,spv::DecorationBufferBlock))
            binding.type             =  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        else
            binding.type             =  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

        bindings.push_back(binding);
    }

    for (auto& resource: resources.subpass_inputs)
    {
        BindingInfo binding;

        binding.name             =  compiler.get_name(resource.id);
        binding.set              =  compiler.get_decoration(resource.id,spv::DecorationDescriptorSet);
        binding.binding          =  compiler.get_decoration(resource.id,spv::DecorationBinding);
        binding.type             =  VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        binding.shaderStage      =  ShaderTypeToStage(type);
        binding.updateFrequency  =  SetToFrequency(binding.set);

        bindings.push_back(binding);
    }
*/
}

void Descriptor::PrintBindings()
{
    std::cout << "\n";

    for (auto binding: bindings)
    {
        std::cout << "Binding name = " << binding.name << "  set = " << binding.set << "  binding = " << binding.binding << "  shader stage = " << std::to_string(binding.shaderStage);
    }
}


VkShaderStageFlags Descriptor::ShaderTypeToStage(ShaderType type)
{
    if (type == ShaderType::vert)
        return VK_SHADER_STAGE_VERTEX_BIT;
    if (type==ShaderType::frag)
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    if (type==ShaderType::comp)
        return VK_SHADER_STAGE_COMPUTE_BIT;

    return VK_SHADER_STAGE_ALL;
}

DescriptorUpdateFrequency Descriptor::SetToFrequency(uint32_t set)
{
    if (set==0)
        return DescriptorUpdateFrequency::PerFrame;
    else if (set==1)
        return DescriptorUpdateFrequency::PerPass;
    else if (set==2)
        return DescriptorUpdateFrequency::PerMaterial;
    else if (set==3)
        return DescriptorUpdateFrequency::PerObject;
    else
        return DescriptorUpdateFrequency::None; // for error handling in future
}

