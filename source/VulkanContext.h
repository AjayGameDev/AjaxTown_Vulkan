#pragma once

#include "Window.h"
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

class VulkanContext 
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



          Window* window;

          explicit VulkanContext(Window* window);
          ~VulkanContext();

          // No copy
          VulkanContext(const VulkanContext&) = delete;
          VulkanContext& operator=(const VulkanContext&) = delete;

          // No move
          VulkanContext(VulkanContext&&) = delete;
          VulkanContext& operator=(VulkanContext&&) = delete;

          void CreateInstance();
          void CreateSurface();
          void PickPhysicalDevice();
          void CheckPhysicalDeviceForRequiredQueues();
          void CreteLogicalDevice();
          void CheckSurfaceCapabilities(); // Select best available format, colorspace and present mode
          void CreateGlobalAllocator();

};
