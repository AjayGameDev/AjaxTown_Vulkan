#pragma once



class Window 
{
  private:
          bool shouldCloseWindow = false;
          SDL_Window* handle;
          SDL_Gamepad* gamepad = nullptr;
  public:
          Window(const char* title, const int width, const int height);
          ~Window();
          char const* const* GetExtensions(uint32_t& extensionCount);
          void CreateSurface(VkInstance instance, VkSurfaceKHR& surface);
          bool ShouldCloseWindow();
          void GetInput(float& deltaX,float& deltaY,float& targetDistance,float& x,float& y, float& z);


};
