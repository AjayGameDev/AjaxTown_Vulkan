#pragma once


//#include "SDL3/SDL.h"
//#include "SDL3/SDL_vulkan.h"
#include "vulkan/vulkan.h"

#include <fstream>
#include <iostream>
#include <vector>

//#include "BufferManager.h"
#include "GameTime.h"
#include "Model.h"
#include "Window.h"

#define VMA_IMPLEMENTATION
#include "Dependencies/vk_mem_alloc.h"

#include "iomanip" // to set precision because cout has precision of 7 digits which fine for float but for double use
                  //  std::cout << std::setprecision(17) << time.deltaTime
                 //   to be remove later

#ifndef NDEBUG
#include "spdlog/spdlog.h" // include only when debugging [ when NDBUG is not defined ]
#endif




