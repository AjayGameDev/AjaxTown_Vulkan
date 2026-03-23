#include "Window.h"

Window::Window(const char *title, const int width, const int height)
{

  std::string error;

  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    error = SDL_GetError();
    throw std::runtime_error("Can't Initialize SDL \n" + error);
  }

  handle = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE  | SDL_WINDOW_BORDERLESS | SDL_WINDOW_TRANSPARENT);
  //handle = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

  if (handle == nullptr)
  {
    error = SDL_GetError();

    SDL_Quit();
    throw std::runtime_error("Can't create SDL window \n" + error);

  }
}

char const *const *Window::GetExtensions(uint32_t &extensionCount)
{
  return SDL_Vulkan_GetInstanceExtensions(&extensionCount);
}
void Window::CreateSurface(VkInstance instance, VkSurfaceKHR& surface)
{
  if (!SDL_Vulkan_CreateSurface(handle,instance,nullptr,&surface))
  {
    throw std::runtime_error("\nCan't create SDL surface!");
  }
}

bool Window::ShouldCloseWindow()
{
  SDL_Event e;
  while (SDL_PollEvent(&e))
  {
    if (e.type == SDL_EVENT_QUIT)
      shouldCloseWindow = true;
  }

  return shouldCloseWindow;
}

Window::~Window()
{
  SDL_DestroyWindow(handle);
  SDL_Quit();
}
