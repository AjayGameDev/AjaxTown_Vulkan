#pragma once




class Shader 
{
    private:
            Context&       context;
            std::string    shaderName;
            ShaderType     shaderType;
            VkShaderModule vertexShaderModule = VK_NULL_HANDLE,fragmentShaderModule = VK_NULL_HANDLE,computeShaderModule = VK_NULL_HANDLE;

            VkShaderModule CreateShaderModule(const VkDevice &device, const std::vector<uint32_t> &shaderCode);

    public:
            Shader(Context& context);
            Shader(Context& context,const char *shaderName,ShaderType shaderType);
            VkShaderModule& GetVertexShaderModule()   {  return vertexShaderModule;   }
            VkShaderModule& GetFragmentShaderModule() {  return fragmentShaderModule; }
            VkShaderModule& GetComputeShaderModule()  {  return computeShaderModule;  }
            std::vector<uint32_t> ReadShader(const std::string& shaderName, ShaderType shaderType);
            void LoadShader(const char *shaderName, Context& context);
            ~Shader();

};




