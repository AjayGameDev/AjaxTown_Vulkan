#pragma once
#include "vulkan/vulkan.h"
#include "Context.h"
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
            Context&       context;
            std::string    shaderName;
            ShaderType     shaderType;
            VkShaderModule vertexShaderModule,fragmentShaderModule;

            std::vector<char> ReadShader(const std::string& shaderName, ShaderType shaderType);
            VkShaderModule CreateShaderModule(const VkDevice& device,const std::vector<char>& shaderCode);

    public:
            Shader(Context& context);
            Shader(Context& context,const char *shaderName);
            VkShaderModule& GetVertexShaderModule() { return vertexShaderModule; }
            VkShaderModule& GetFragmentShaderModule() { return fragmentShaderModule; }
            void LoadShader(const char *shaderName, Context& context);
            ~Shader();

};




