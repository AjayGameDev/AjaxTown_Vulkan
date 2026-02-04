#pragma once

class Buffer; // forward declaration
class Framebuffer;

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

    void UpdateGlobalSet(Buffer& vertexBuffer,Framebuffer& frameBuffer);
    void UpdateComputeSet(Buffer& indirectBuffer);

    VkDescriptorSet& GetGlobalSet() { return globalSet; }
    VkDescriptorSet& GetComputeSet() { return computeSet; }

    VkDescriptorSetLayout& GetGlobalSetLayout() { return globalSetLayout; }
    VkDescriptorSetLayout& GetComputeSetLayout() { return computeSetLayout; }

};
