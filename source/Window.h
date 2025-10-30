#pragma once

#include "vulkan/vulkan.h"

class SDL_Window;

class Window 
{
  private:
          bool shouldCloseWindow = false;
          SDL_Window* handle;
  public:
          Window(const char* title, const int width, const int height);
          ~Window();
          char const* const* GetExtensions(uint32_t& extensionCount);
          void CreateSurface(VkInstance instance, VkSurfaceKHR& surface);
          bool ShouldCloseWindow();


};
