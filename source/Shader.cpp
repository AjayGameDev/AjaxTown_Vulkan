#include "Shader.h"

#include "FileManager.h"


Shader::Shader(Context& context): context(context)
{
    shaderName            =  "";
    shaderType            =  ShaderType::none;
    vertexShaderModule    =  nullptr;
    fragmentShaderModule  =  nullptr;
    computeShaderModule   =  nullptr;
}

Shader::Shader(Context& context,const char *shaderName,ShaderType shaderType): context(context),shaderName(shaderName),shaderType(shaderType)
{
    LoadShader(shaderName,context);
}

void Shader::LoadShader(const char *shaderName, Context& context)
{
    if (shaderName=="")
        this->shaderName = shaderName;
    //if (context==nullptr)
    //    this->context = context;

    if (shaderType == ShaderType::vertfrag)
    {
        auto vertexShaderCode    =  ReadShader(shaderName,ShaderType::vert);
        auto fragmentShaderCode  =  ReadShader(shaderName,ShaderType::frag);

        vertexShaderModule   = CreateShaderModule(context.device,vertexShaderCode);
        fragmentShaderModule = CreateShaderModule(context.device,fragmentShaderCode);
    }
    else if (shaderType == ShaderType::comp)
    {
        auto computeShaderCode    =  ReadShader(shaderName,ShaderType::comp);

        computeShaderModule   = CreateShaderModule(context.device,computeShaderCode);
    }




}

std::vector<uint32_t> Shader::ReadShader(const std::string &shaderName, ShaderType shaderType)
{
    std::string shaderExtension;

    switch (shaderType)
    {

        case ShaderType::vert     :   shaderExtension  =  ".vert.spv";    break;     // these extensions are not enforced by the vulkan but
        case ShaderType::frag     :   shaderExtension  =  ".frag.spv";    break;    //  tools like glslc (from shaderc compiler) uses these extensions
        case ShaderType::comp     :   shaderExtension  =  ".comp.spv";    break;   //   these can be overriden while compiling like using .vertex instead of .vert
        default                   :   shaderExtension  =  "";             break;  //    but then you have to override it for all (~14) extensions

    }

    //std::string filePath = R"(C:\Users\dubey\CLionProjects\AjaxTown\assets\shaders\compiled\)" + shaderName + shaderTypeName;
    std::string filePath = FileManager::GetLocation(FileManager::Shader) + shaderName + shaderExtension;

    std::vector<uint32_t> buffer = FileManager::LoadBytes_32bit(filePath);

    return buffer;
    /*
    std::ifstream file(filePath,std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cout << "\nCan't open shader file!" << shaderName;
    }
    const long long fileSize = file.tellg();
    size_t wordCount = fileSize / sizeof(uint32_t);
    std::vector<uint32_t> buffer(wordCount);

    file.seekg(0);
    file.read(reinterpret_cast<char *>(buffer.data()),fileSize);
    file.close();
*/

    //SDL_IOStream* io = SDL_IOFromFile(filePath.c_str(),"rb");
//
    //if (!io)
    //{
    //    std::cout << "\nCan't open shader file!" << shaderName;
    //}
//
    //Sint64 fileSize = SDL_GetIOSize(io);
    //std::vector<uint32_t> buffer(fileSize/sizeof(uint32_t));
    //SDL_ReadIO(io,buffer.data(),fileSize);
    //SDL_CloseIO(io);

    //return buffer;
}

VkShaderModule Shader::CreateShaderModule(const VkDevice& device, const std::vector<uint32_t>& shaderCode)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};

    shaderModuleCreateInfo.sType     =  VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize  =  shaderCode.size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode     =  shaderCode.data();

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device,&shaderModuleCreateInfo,nullptr,&shaderModule) != VK_SUCCESS)
        std::cout << "\nCan't create shader module!";

    return shaderModule;
}


Shader::~Shader()
{
    if (vertexShaderModule!=VK_NULL_HANDLE)
        vkDestroyShaderModule(context.device,vertexShaderModule,nullptr);
    if (fragmentShaderModule!=VK_NULL_HANDLE)
        vkDestroyShaderModule(context.device,fragmentShaderModule,nullptr);
    if (computeShaderModule!=VK_NULL_HANDLE)
        vkDestroyShaderModule(context.device,computeShaderModule,nullptr);
}
