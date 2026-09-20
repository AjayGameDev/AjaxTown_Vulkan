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
//#if defined(__ANDROID__)
#if defined(TARGET_PLATFORM_ANDROID)
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

void Window::GetInput(float& deltaX,float& deltaY,float& targetDistance,float& x,float& y, float& z,bool& focusCameraPressed)
{


 deltaX = deltaY = x = y = 0;


  SDL_Event e;


  while (SDL_PollEvent(&e))
  {
    SDL_Keymod keymods = SDL_GetModState();

    bool altPressed = (keymods & SDL_KMOD_ALT) != 0;
    bool shiftPressed = (keymods & SDL_KMOD_SHIFT) != 0;
    bool leftMousePressed = (e.motion.state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;


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
      if (altPressed && leftMousePressed)
      {
        if (shiftPressed)
        {
          x -= e.motion.xrel;
          y += e.motion.yrel;
        }
        else
        {
          deltaX += e.motion.xrel;
          deltaY += e.motion.yrel;
        }
      }

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
      if      (e.wheel.y > 0)      targetDistance -= .1f;
      else if (e.wheel.y < 0)      targetDistance += .1f;

    }
    else if (e.type == SDL_EVENT_PINCH_UPDATE)
    {
      targetDistance *= e.pinch.scale;
    }

    if (e.type == SDL_EVENT_KEY_DOWN)
    {
      //if (e.key.key==SDLK_D)
      //  x += .1f;
      //else if (e.key.key == SDLK_A)
      //  x -= .1f;
      //else if (e.key.key == SDLK_W)
      //  y += .1f;
      //else if (e.key.key == SDLK_S)
      //  y -= .1f;
      //else if (e.key.key == SDLK_J)
      //  z += .1f;
      //else if (e.key.key == SDLK_L)
      //  z -= .1f;
      if (e.key.key == SDLK_F)
        focusCameraPressed = true;

    }

  }

  if (gamepad)
  {
    // Poll events will give discrete value for gamepad as it fires only when value is changed, this will feels smooth like mouse and touch [mouse and touch are working fine in polling as they use relative position that accumulates over time and gives smooth result]
    float horizontalRightAxis = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_RIGHTX) / 32767.0f; // to get value btw 0 and 1
    float verticalRightAxis   = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_RIGHTY) / 32767.0f;

    float horizontalLeftAxis = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_LEFTX)  / 32767.0f;
    float verticalLeftAxis   = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_LEFTY) / 32767.0f;

    float leftTrigger  = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_LEFT_TRIGGER)  / 32767.0f;
    float rightTrigger = SDL_GetGamepadAxis(gamepad,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) / 32767.0f;

    float deadzone = .05f; // to avoid jitter when controller is idle

    if(std::abs(horizontalRightAxis) < deadzone)   horizontalRightAxis = 0;
    if(std::abs(verticalRightAxis)   < deadzone)   verticalRightAxis   = 0;

    if(std::abs(horizontalLeftAxis) < deadzone) horizontalLeftAxis = 0;
    if(std::abs(verticalLeftAxis)   < deadzone) verticalLeftAxis   = 0;

    if(std::abs(leftTrigger)    < deadzone) leftTrigger    = 0;
    if(std::abs(rightTrigger)   < deadzone) rightTrigger   = 0;

    horizontalRightAxis = std::copysign(horizontalRightAxis * horizontalRightAxis,horizontalRightAxis); // magnitude of first argument and sign of the second
    verticalRightAxis   = std::copysign(verticalRightAxis   * verticalRightAxis,  verticalRightAxis);

    horizontalLeftAxis = std::copysign(horizontalLeftAxis * horizontalLeftAxis,horizontalLeftAxis); // magnitude of first argument and sign of the second
    verticalLeftAxis   = std::copysign(verticalLeftAxis * verticalLeftAxis,verticalLeftAxis);

    leftTrigger    =  std::copysign(leftTrigger  * leftTrigger, leftTrigger); // magnitude of first argument and sign of the second
    rightTrigger   =  std::copysign(rightTrigger * rightTrigger,rightTrigger);

    deltaX += horizontalRightAxis * 10.0f;
    deltaY += verticalRightAxis   * 10.0f;

    x += horizontalLeftAxis;
    y -= verticalLeftAxis  ;

    targetDistance += leftTrigger  * .001f;
    targetDistance -= rightTrigger * .001f;

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
