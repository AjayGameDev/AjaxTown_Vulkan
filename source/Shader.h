#pragma once
#include "vulkan/vulkan.h"
#include "VulkanContext.h"
#include "string"
#include "vector"

enum class ShaderType : uint8_t
{
    none,
    vert,
    frag,
    comp

};


class Shader 
{
    private:
            std::string    shaderName;
            ShaderType     shaderType;
            VulkanContext& context;
            VkShaderModule vertexShaderModule,fragmentShaderModule;

            std::vector<char> ReadShader(const std::string& shaderName, ShaderType shaderType);
            VkShaderModule CreateShaderModule(const VkDevice& device,const std::vector<char>& shaderCode);

    public:
            Shader();
            Shader(const char *shaderName, VulkanContext& context);
            void LoadShader(const char *shaderName, VulkanContext& context);
            ~Shader();

};




