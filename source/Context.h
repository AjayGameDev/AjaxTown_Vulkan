#pragma once


class Window;



class Context
{
  public:
          VkInstance                instance         =   VK_NULL_HANDLE;
          VkSurfaceKHR              surface          =   VK_NULL_HANDLE;
          VkPhysicalDevice          physicalDevice   =   VK_NULL_HANDLE;
          VkDevice                  device           =   VK_NULL_HANDLE;
          VmaAllocator              allocator        =   VK_NULL_HANDLE;
          VkDebugUtilsMessengerEXT  debugMessenger   =   VK_NULL_HANDLE;


          VkQueue graphicsQueue = VK_NULL_HANDLE;
          VkQueue transferQueue = VK_NULL_HANDLE;
          VkQueue computeQueue  = VK_NULL_HANDLE;

          int graphicsFamilyIndex;
          int transferFamilyIndex;
          int computeFamilyIndex;

          VkSurfaceFormatKHR format{};                        // selected format for the swapchain
          VkPresentModeKHR presentMode{};                    // FIFO or Mailbox basically double vs triple buffering
          VkSurfaceCapabilitiesKHR surfaceCapabilities{};   // we need to go throgh supported formats and present modes to select the right one



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
