#pragma once

#include <cstdint>
#include "SDL3/SDL.h"
#include "SDL3/SDL_vulkan.h"

class Window 
{

  public:
          Window(const char* title, const int width, const int height);
          ~Window();
          char const* const* GetExtensions(uint32_t& extensionCount);
          void CreateSurface(VkInstance instance,VkSurfaceKHR surface);

          SDL_Window* window;


};
