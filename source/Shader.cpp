#include "Shader.h"
#include <fstream>
#include "iostream"



Shader::Shader(Context& context): context(context)
{
    shaderName = "";
    shaderType = ShaderType::none;
    vertexShaderModule = nullptr;
    fragmentShaderModule = nullptr;
}

Shader::Shader(Context& context,const char *shaderName): context(context),shaderName(shaderName)
{
    LoadShader(shaderName,context);
}

void Shader::LoadShader(const char *shaderName, Context& context)
{
    if (shaderName=="")
        this->shaderName = shaderName;
    //if (context==nullptr)
    //    this->context = context;

    auto vertexShaderCode    =  ReadShader(shaderName,ShaderType::vert);
    auto fragmentShaderCode  =  ReadShader(shaderName,ShaderType::frag);

    vertexShaderModule   = CreateShaderModule(context.device,vertexShaderCode);
    fragmentShaderModule = CreateShaderModule(context.device,fragmentShaderCode);

}

std::vector<char> Shader::ReadShader(const std::string &shaderName, ShaderType shaderType)
{
    std::string shaderTypeName;

    switch (shaderType)
    {

        case ShaderType::vert     :   shaderTypeName  =  ".vert.spv";    break;     // these extensions are not enforced by the vulkan but
        case ShaderType::frag     :   shaderTypeName  =  ".frag.spv";    break;    //  tools like glslc (from shaderc compiler) uses these extensions
        case ShaderType::comp     :   shaderTypeName  =  ".comp.spv";    break;   //   these can be overriden while compiling like using .vertex instead of .vert
        default                   :   shaderTypeName  =  "";             break;  //    but then you have to override it for all (~14) extensions

    }

    std::string filePath = R"(C:\Users\dubey\CLionProjects\AjaxTown\assets\shaders\compiled\)" + shaderName + shaderTypeName;

    std::ifstream file(filePath,std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cout << "\nCan't open shader file!" << shaderName;
    }
    const long long fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(),fileSize);
    file.close();

    return buffer;
}

VkShaderModule Shader::CreateShaderModule(const VkDevice& device, const std::vector<char>& shaderCode)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};

    shaderModuleCreateInfo.sType     =  VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize  =  shaderCode.size();
    shaderModuleCreateInfo.pCode     =  reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device,&shaderModuleCreateInfo,nullptr,&shaderModule) != VK_SUCCESS)
        std::cout << "\nCan't create shader module!";

    return shaderModule;
}


Shader::~Shader()
{
    vkDestroyShaderModule(context.device,vertexShaderModule,nullptr);
    vkDestroyShaderModule(context.device,fragmentShaderModule,nullptr);
}
