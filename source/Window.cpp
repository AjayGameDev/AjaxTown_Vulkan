#include "Window.h"

Window::Window(const char *title, const int width, const int height)
{

  std::string error;
  SDL_SetHint(SDL_HINT_ORIENTATIONS,"LandscapeLeft LandscapeRight"); // otherwise game will open in potrait mode at system level even if you set landscape mode in manifest file
  // set hint before initialization
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
  {
    error = SDL_GetError();
    throw std::runtime_error("Can't Initialize SDL \n" + error);
  }
#if defined(__ANDROID__)
        // SDL_WINDOW_FULLSCREEN locks fps to 60 for some reason SDL_WINDOW_MAXIMIZED can push to 120
        handle = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN  | SDL_WINDOW_MAXIMIZED);//SDL_WINDOW_RESIZABLE ); //  | SDL_WINDOW_BORDERLESS| SDL_WINDOW_TRANSPARENT);
  #else
        handle = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_FULLSCREEN ); //  | SDL_WINDOW_BORDERLESS| SDL_WINDOW_TRANSPARENT);
#endif

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

void Window::GetInput(float& deltaX,float& deltaY,float& targetDistance,float& x,float& y, float& z)
{


 deltaX = deltaY = 0;


  SDL_Event e;
  while (SDL_PollEvent(&e))
  {
    if (e.type == SDL_EVENT_QUIT)
      shouldCloseWindow = true;

    if (e.type == SDL_EVENT_GAMEPAD_ADDED)
    {
      if (!gamepad)
        gamepad = SDL_OpenGamepad(e.gdevice.which);
      spdlog::info("Gamepad Detected!");
    }
    else if (e.type == SDL_EVENT_GAMEPAD_REMOVED)
    {
      if (gamepad)
      {
        SDL_CloseGamepad(gamepad);
        gamepad = nullptr;
        spdlog::info("Gamepad removed!");
      }
    }


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
   // else if (e.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
   // {
   //   float value = e.gaxis.value / 32767.0f; // normalize to get [-1,1]
   //   if (e.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTX)
   //     deltaX += value * 100.0f;
   //   else if (e.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTY)
   //     deltaY += value * 100.0f;
   // }


    //else
    //  deltaX = deltaY = 0;

    if (e.type == SDL_EVENT_MOUSE_WHEEL)
    {
      if      (e.wheel.y > 0)      targetDistance += .1f;
      else if (e.wheel.y < 0)      targetDistance -= .1f;

    }
    else if (e.type == SDL_EVENT_PINCH_UPDATE)
    {
      targetDistance *= e.pinch.scale;
    }

    if (e.type == SDL_EVENT_KEY_DOWN)
    {
      if (e.key.key==SDLK_D)
        x += .1f;
      else if (e.key.key == SDLK_A)
        x -= .1f;
      else if (e.key.key == SDLK_W)
        y += .1f;
      else if (e.key.key == SDLK_S)
        y -= .1f;
      else if (e.key.key == SDLK_J)
        z += .1f;
      else if (e.key.key == SDLK_L)
        z -= .1f;

    }

  }

  if (gamepad)
  {
    // Poll events will give discrete value for gamepad as it fires only when value is changed, this will feels smooth like mouse and touch [mouse and touch are working fine in polling as they use relative position that accumulates over time and gives smooth result]
    float horizontalAxis = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_RIGHTX) / 32767.0f;
    float verticalAxis = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_RIGHTY) / 32767.0f;

    float deadzone = .05f; // to avoid jitter when controller is idle

    if(std::abs(horizontalAxis) < deadzone) horizontalAxis = 0;
    if(std::abs(verticalAxis) < deadzone) verticalAxis = 0;

    horizontalAxis = std::copysign(horizontalAxis * horizontalAxis,horizontalAxis); // magnitude of first argument and sign of the second
    verticalAxis = std::copysign(verticalAxis * verticalAxis,verticalAxis);

    deltaX += horizontalAxis * 10.0f;
    deltaY += verticalAxis * 10.0f;

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
