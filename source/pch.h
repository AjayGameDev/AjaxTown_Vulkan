#pragma once


#include "vulkan/vulkan.h"
#include "spdlog/spdlog.h"
#include "Dependencies/vk_mem_alloc.h"
#include "stdexcept"
#include "string"
#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory.h>
#include "Maths.h"
#include "Context.h"


enum class ShaderType : uint8_t
{
    none,
    vert,
    frag,
    comp,
    vertfrag

};

enum class DescriptorUpdateFrequency
{
    PerFrame,
    PerPass,
    PerMaterial,
    PerObject,
    None
};
