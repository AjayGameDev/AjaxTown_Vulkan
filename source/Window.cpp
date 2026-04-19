#include "Window.h"

Window::Window(const char *title, const int width, const int height)
{

  std::string error;

  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    error = SDL_GetError();
    throw std::runtime_error("Can't Initialize SDL \n" + error);
  }
  SDL_SetHint(SDL_HINT_ORIENTATIONS,"LandscapeLeft"); // otherwise game will open in potrait mode at system level even if you set landscape mode in manifest file
  handle = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN ); //  | SDL_WINDOW_BORDERLESS| SDL_WINDOW_TRANSPARENT);
  //handle = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

  if (handle == nullptr)
  {
    error = SDL_GetError();

    SDL_Quit();
    throw std::runtime_error("Can't create SDL window \n" + error);

  }

  SDL_SetWindowRelativeMouseMode(handle,true); // Lock cursor
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

void Window::GetInput(float& deltaX,float& deltaY,float& targetDistance)
{
 deltaX = deltaY = 0;

  SDL_Event e;
  while (SDL_PollEvent(&e))
  {
    if (e.type == SDL_EVENT_QUIT)
      shouldCloseWindow = true;

    if (e.type == SDL_EVENT_MOUSE_MOTION)
    {
      deltaX += e.motion.xrel;
      deltaY += e.motion.yrel;
    }
    else if (e.type == SDL_EVENT_FINGER_MOTION)
    {
      deltaX += e.tfinger.dx * 2500.0f; // [-1,1]
      deltaY += e.tfinger.dy * 2500.0f;
    }
    //else
    //  deltaX = deltaY = 0;

    if (e.type == SDL_EVENT_MOUSE_WHEEL)
    {
      if      (e.wheel.y > 0)      targetDistance += .1f;
      else if (e.wheel.y < 0)      targetDistance -= .1f;

    }


  }

}

bool Window::ShouldCloseWindow()
{


  return shouldCloseWindow;


}


Window::~Window()
{
  SDL_DestroyWindow(handle);
  SDL_Quit();
}
