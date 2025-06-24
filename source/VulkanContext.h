#pragma once

#include "Window.h"
#include "vulkan/vulkan.h"
/*
class VulkanContext 
{
  public:
          VkInstance        instance;
          VkSurfaceKHR      surface;
          VkDevice          device;
          VkPhysicalDevice  physicalDevice;

          VkQueue graphicsQueue;
          VkQueue transferQueue;
          VkQueue computeQueue;

          int graphicsFamilyIndex;
          int transferFamilyIndex;
          int computeFamilyIndex;

          Window* window;

          VulkanContext(Window* window);
          ~VulkanContext();
          void CreateInstance();
          void CreateSurface();
          void PickPhysicalDevice();
          void CheckPhysicalDeviceForRequiredQueues();
          void CreteLogicalDevice();

};
*/