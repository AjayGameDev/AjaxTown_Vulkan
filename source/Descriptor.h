#pragma once

class Buffer; // forward declaration

struct BindingInfo
{
    std::string name;
    uint32_t set;
    uint32_t binding;
    VkDescriptorType type;
    VkShaderStageFlags shaderStage;
    DescriptorUpdateFrequency updateFrequency;
};

class Descriptor 
{
    Context& context;
    uint32_t slot_global_textureArray = -1, slot_global_inputAttachment = -1, slot_global_samplers = -1; // update first then return the value as id
    VkDescriptorPool descriptorPool{};

    VkDescriptorSetLayout globalSetLayout{};
    VkDescriptorSetLayout computeSetLayout{};

    VkDescriptorSet globalSet{};
    VkDescriptorSet computeSet{};

    std::vector<BindingInfo> bindings;
    VkShaderStageFlags ShaderTypeToStage(ShaderType type);
    DescriptorUpdateFrequency SetToFrequency(uint32_t set);

public:
    explicit Descriptor(Context& context);
    ~Descriptor(); // This should be destroyed after pipeline and pipeline layout
    void ReflectBindings(const std::vector<uint32_t>& spirvCode,ShaderType type);
    void PrintBindings();

    void CreateMemoryPool();

    void CreateGlobalSetLayout();
    void CreateComputeSetLayout();

    void AllocateGlobalSet();
    void AllocateComputeSet();

    void UpdateComputeSet(Buffer& indirectBuffer,Buffer& countBuffer);

    uint32_t RegisterImage_Global_TextureArray(VkImageView& imageview);
    uint32_t RegisterImage_Global_InputAttachment(VkImageView& imageview);
    uint32_t RegisterSampler_Global(VkSampler& sampler);

    VkDescriptorSet& GetGlobalSet() { return globalSet; }
    VkDescriptorSet& GetComputeSet() { return computeSet; }

    VkDescriptorSetLayout& GetGlobalSetLayout() { return globalSetLayout; }
    VkDescriptorSetLayout& GetComputeSetLayout() { return computeSetLayout; }

};
