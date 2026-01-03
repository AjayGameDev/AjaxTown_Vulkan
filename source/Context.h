#pragma once


class Window;

enum class RendererType
{
    Deferred,
    Forward
};

class Context
{
  public:
          VkInstance                instance;
          VkSurfaceKHR              surface;
          VkPhysicalDevice          physicalDevice;
          VkDevice                  device;
          VmaAllocator              allocator;
          VkDebugUtilsMessengerEXT  debugMessenger;


          VkQueue graphicsQueue;
          VkQueue transferQueue;
          VkQueue computeQueue;

          int graphicsFamilyIndex;
          int transferFamilyIndex;
          int computeFamilyIndex;

          VkSurfaceFormatKHR format;                        // selected format for the swapchain
          VkPresentModeKHR presentMode;                    // FIFO or Mailbos basically double vs triple buffering
          VkSurfaceCapabilitiesKHR surfaceCapabilities;   // we need to go throgh supported formats and present modes to select the right one



          Window& window;

          explicit Context(Window& window);
          ~Context();

          // No copy
          Context(const Context&) = delete;
          Context& operator=(const Context&) = delete;

          // No move
          Context(Context&&) = delete;
          Context& operator=(Context&&) = delete;

          void CreateInstance();
          void CreateSurface(VkInstance instance, VkSurfaceKHR& surface);
          void PickPhysicalDevice();
          void CheckPhysicalDeviceForRequiredQueues();
          void CreteLogicalDevice();
          void CheckSurfaceCapabilities(); // Select best available format, colorspace and present mode
          void CreateGlobalAllocator();

};
