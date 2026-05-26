
module;

#include "vulkan/vulkan.h"
//#include "dependencies/vk_mem_alloc.h"
//#include "spdlog/spdlog.h"
//#include "SDL3/SDL.h"
//#include "SDL3/SDL_vulkan.h"
//#include "SDL3/SDL_main.h"
//#include <memory.h>


export module Engine.Core;

export import std;
export import Engine.Maths; // anything that imports engine core will also get math library access
export import Engine.Context;


export enum class RendererType
{
    Deferred,
    Forward
};


export enum class ShaderType : uint8_t // restricts to only 8 values because of bitwise flags other wise you can go from 0-255
{
    none = 0,       // 0000 0000 = 0
    vert = 1<<0,   //  0000 0001 = 1
    frag = 1<<1,  //   0000 0010 = 2
    comp = 1<<2  //    0000 0100 = 4

};


export constexpr VkFormat diffuseFormat    =           VK_FORMAT_R8G8B8A8_SRGB; // Base color in linear space
export constexpr VkFormat normalFormat     =           VK_FORMAT_A2R10G10B10_UNORM_PACK32; // 10 bit normal depth with no alpha
export constexpr VkFormat RMAOFormat       =           VK_FORMAT_R8G8B8A8_UNORM;
export constexpr VkFormat depthFormat      =           VK_FORMAT_D32_SFLOAT;
export constexpr VkFormat hdrFormat        =           VK_FORMAT_R16G16B16A16_SFLOAT;
//#define finalImageFormat            VK_FORMAT_B8G8R8A8_SRGB // Final output [Change later TO DO ]




export inline ShaderType operator|(ShaderType a, ShaderType b)
{
    return static_cast<ShaderType>( static_cast<uint8_t>(a) | static_cast<uint8_t>(b) );
}
// can't overload again, it is mostly used in if statements to check if shaderType has this value or not which expects bool instead of ShaderType
/*
inline ShaderType operator&(ShaderType a, ShaderType b)
{
    return static_cast<ShaderType>( static_cast<uint8_t>(a) & static_cast<uint8_t>(b) );
}
*/
export inline bool operator&(ShaderType a, ShaderType b)
{
    return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

// if you want to remove only one flag from combined flag like bloom/ssao from graphics settings flag then you can do graphicsSettings -= GraphicsSettings::bloom;
export inline ShaderType& operator-=(ShaderType& a, ShaderType b)
{
    a = static_cast<ShaderType>(static_cast<uint8_t>(a) & ~static_cast<uint8_t>(b)); // we are making changes to temp variable b and reference of actual variable a
    return a; // take a as reference as we want to make changes to the actual variable instead of temporary one
}




export enum class DescriptorUpdateFrequency
{
    PerFrame,
    PerPass,
    PerMaterial,
    PerObject,
    None
};
