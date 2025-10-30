#pragma once


//#include "SDL3/SDL.h"
//#include "SDL3/SDL_vulkan.h"
#include "vulkan/vulkan.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "BufferManager.h"
#include "GameTime.h"
#include "Model.h"
#include "Window.h"
#include "Swapchain.h"
#include "Renderpass.h"
#include "Framebuffer.h"
#include "ImageManager.h"
#include "Renderer.h"
#include "Shader.h"
#include "GraphicsPipeline.h"
#include "GraphicsPipelineBuilder.h"

#define VMA_IMPLEMENTATION
//#define VMA_VULKAN_VERSION 1001000  // Vulkan 1.1 (order matters here)
#include "vma/vk_mem_alloc.h"

#include "spdlog/spdlog.h"


#include "iomanip" // to set precision because cout has precision of 7 digits which fine for float but for double use
                  //  std::cout << std::setprecision(17) << time.deltaTime
                 //   to be remove later







