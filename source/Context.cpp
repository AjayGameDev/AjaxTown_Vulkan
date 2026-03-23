#include "Window.h"


#pragma region Debugging Helper Functions

#ifdef NDEBUG

//std::cout << "\n" << "Not Debug Mode "; // Disable validation layers for better optimization
const bool enableValidationLayers = false;

#else

//std::cout << "\n" << "Debug Mode";
constexpr bool enableValidationLayers = true;

#endif

#ifndef NDEBUG  // ndef means NDEBUG(No Debug) is not defined so it is a debug mode

  // These are macros used by vulkan to ensure cross-platform compatibility
 //  On windows VKAPI_CALL will be __declspec(dllexport) and VKAPI_CALL will be __stdcall

  VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,VkDebugUtilsMessageTypeFlagsEXT messageType,const VkDebugUtilsMessengerCallbackDataEXT* callbackData,void* userData)
  {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
      spdlog::error("[Vulkan] {}",callbackData->pMessage);
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
      spdlog::warn("[Vulkan] {}",callbackData->pMessage);
    else
      spdlog::info("[Vulkan] {}",callbackData->pMessage);

    return false;   // Vulkan continue, the message has been reported and there is no need to stop
                   //  If you return true, then vulkan will stop execution
  }

  // It's not in the core vulkan so have to load these extension functions manually to create and destroy Debug Messenger
  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
  {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
      return func(instance, pCreateInfo, pAllocator, pMessenger);
    else
      return VK_ERROR_EXTENSION_NOT_PRESENT;

  }

  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
  {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr)
      func(instance, messenger, pAllocator);
  }

#endif


#pragma endregion


#pragma region Initialize Vulkan


Context::Context(Window& window) : window(window)
{
    CreateInstance();
    CreateSurface(instance,surface);
    PickPhysicalDevice();
    CheckSurfaceCapabilities();
    CheckPhysicalDeviceForRequiredQueues();
    CreteLogicalDevice();
    CreateGlobalAllocator();
}


void Context ::CreateInstance()
{
    //---------------------------------------------------------------------------- Describe your app info

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Ajax Town";
    appInfo.applicationVersion = 1.0;
    appInfo.pEngineName = "Ajax";
    appInfo.engineVersion = 1.0;
    appInfo.apiVersion = VK_MAKE_API_VERSION(0,1,3,0); // Vulkan version 1.1 supports most of the android [ switched to 1.3 ]

    //---------------------------------------------------------------------------- Get all available extensions

    uint32_t extensionsCount;
    const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    char const *const *extensions = window.GetExtensions(extensionsCount);
    std::vector<const char *> extensionsVector(extensions, extensions + extensionsCount);

    if (enableValidationLayers)
      extensionsVector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    if (extensionsVector.empty())
      throw std::runtime_error("Can't load any vulkan extension! \n");

    //----------------------------------------------------------------------------
    //Vulkan instance creation info

    VkInstanceCreateInfo createInfo = {};
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionsVector.size();
    createInfo.ppEnabledExtensionNames = extensionsVector.data();

#ifndef NDEBUG
    if (enableValidationLayers)
    {
      createInfo.enabledLayerCount   = validationLayers.size();
      createInfo.ppEnabledLayerNames = validationLayers.empty() ? nullptr : validationLayers.data();

      debugCreateInfo.sType             =   VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      debugCreateInfo.messageSeverity   =   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
      debugCreateInfo.messageType       =   VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      debugCreateInfo.pfnUserCallback   =   DebugCallback;
      debugCreateInfo.pUserData         =   nullptr;

      createInfo.pNext = &debugCreateInfo;
    }
#else
  createInfo.pNext = nullptr;
#endif

    //----------------------------------------------------------------------------
    // Vulkan instance creation

    instance = nullptr;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
      throw std::runtime_error("\nCan't create vulkan instance ");



    if (enableValidationLayers)
    {
      // Create a message only if validation layers are enabled
#ifndef NDEBUG
      if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("\nCan't create debug messenger");
#endif
    }
  }

void Context::CreateSurface(VkInstance instance, VkSurfaceKHR& surface)
{
    window.CreateSurface(instance,surface);
}

void Context::CheckSurfaceCapabilities()
{
    // Query surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,surface,&surfaceCapabilities);

    // Query surface formats
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,surface,&surfaceFormatCount,nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,surface,&surfaceFormatCount,surfaceFormats.data());

    // Query present modes
    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,surface,&presentModesCount,nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,surface,&presentModesCount,presentModes.data());

    // Selecting surface format
    format = surfaceFormats[0];
    for (int i=0; i<surfaceFormatCount; i++)
    {
      if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        format = surfaceFormats[i];
        break;
      }
    }

    // Selecting present mode
    presentMode = VK_PRESENT_MODE_FIFO_KHR; // Present as fast as possible | tearing-high chance | latency-low
    for (int i=0; i<presentModesCount; i++)
    {
      if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) // triple buffering | tearing-low chance | latency-bit higher
      {
        presentMode = presentModes[i];
        break;
      }
    }


}

void Context::PickPhysicalDevice()
{
  physicalDevice = nullptr;
  uint32_t deviceCount = 0;

  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr); // query how many physical devices are present
  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data()); // Get all physical devices

  for (const auto device : physicalDevices)
  {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    /*
    VkSampleCountFlags sampleCountFlag =
    deviceProperties.limits.framebufferColorSampleCounts;

    if (sampleCountFlag & VK_SAMPLE_COUNT_32_BIT)
      std::cout << "\n" << "32 Sample count supported";
    else if (sampleCountFlag & VK_SAMPLE_COUNT_16_BIT)
      std::cout << "\n" << "16 Sample count supported";
    else if (sampleCountFlag & VK_SAMPLE_COUNT_8_BIT)
      std::cout << "\n" << "8 Sample count supported";
    else if (sampleCountFlag & VK_SAMPLE_COUNT_4_BIT)
      std::cout << "\n" << "4 Sample count supported";
*/

    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    std::cout << "\n" << deviceProperties.deviceName;
    std::cout << "\n" << deviceFeatures.multiViewport;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      physicalDevice = device;
      break;
    }
  }
  if (physicalDevice == nullptr)
  {
    throw std::runtime_error("\nNo physical device detected with vulkan support!");
  }
}

void Context::CheckPhysicalDeviceForRequiredQueues()
{
  // After picking the physical device, we need to check if it supports the
  // queue that we want to use it for
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

  graphicsFamilyIndex = -1;
  transferFamilyIndex = -1;
  computeFamilyIndex = -1;

  // Pick the first ideal queue and stick to it, no need to override
  for (int i = 0; i < queueFamilyCount; ++i)
  {
    if ((graphicsFamilyIndex == -1) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
    {
      graphicsFamilyIndex = i;
    }
    else if ((transferFamilyIndex == -1) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
    {
      transferFamilyIndex = i;
    }
    else if ((computeFamilyIndex == -1) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
    {
      computeFamilyIndex = i;
    }
  }
   // We can use separate compute queue for async compute so we can improve gpu utilization or dedicated transfer queue to not stutter while uploading data to vram
   // but on android/ios we might only have 1 queue for all tasks so we need to keep that in mind

  if (graphicsFamilyIndex == -1)
    std::cout << "Selected device doesn't support graphics queue!";
  if (transferFamilyIndex == -1)
    std::cout << "Selected device doesn't support transfer queue!";
  if (computeFamilyIndex == -1)
    std::cout << "Selected device doesn't support compute queue!";

  // Checking for presentation support
  VkBool32 presentationSupport = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamilyIndex, surface, &presentationSupport);
  if (!presentationSupport)
  {
    throw std::runtime_error("\nSelected physical device can't present to the surface!");
  }
}

void Context::CreteLogicalDevice()
{
    // After selecting the physical device and getting graphics queue we can create a logical device
    // On mobile GPU graphics,transfer and compute queues are same(most of the time) but different on discrete gpus like nvidia or amd
    device = nullptr;
    float queuePriority = 1.0f;

    //const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME }; // it's not part of vulkan core so have to enable the swap-chain as an extension
    const std::vector<const char*> requiredDeviceExtensions = {  VK_KHR_SWAPCHAIN_EXTENSION_NAME  }; // it's not part of vulkan core, so have to enable the swap-chain as an extension
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    if (graphicsFamilyIndex!=-1)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType             =  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex  =  graphicsFamilyIndex;
        queueCreateInfo.queueCount        =  1;
        queueCreateInfo.pQueuePriorities  =  &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);

    }

    if (transferFamilyIndex!=-1 && transferFamilyIndex!=graphicsFamilyIndex && transferFamilyIndex!=computeFamilyIndex)
    {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType             =  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex  =  transferFamilyIndex;
      queueCreateInfo.queueCount        =  1;
      queueCreateInfo.pQueuePriorities  =  &queuePriority;

      queueCreateInfos.push_back(queueCreateInfo);
    }
    else
      transferFamilyIndex = -1;

    if (computeFamilyIndex!=-1 && computeFamilyIndex!=graphicsFamilyIndex && computeFamilyIndex!=transferFamilyIndex)
    {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType             =  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex  =  computeFamilyIndex;
      queueCreateInfo.queueCount        =  1;
      queueCreateInfo.pQueuePriorities  =  &queuePriority;

      queueCreateInfos.push_back(queueCreateInfo);
    }
    else
      computeFamilyIndex = -1;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

    features12.drawIndirectCount                              =   VK_TRUE;
    features12.bufferDeviceAddress                            =   VK_TRUE;
    features12.descriptorIndexing                             =   VK_TRUE;
    features12.runtimeDescriptorArray                         =   VK_TRUE;
    features12.descriptorBindingPartiallyBound                =   VK_TRUE;
    features12.descriptorBindingVariableDescriptorCount       =   VK_TRUE;
    features12.descriptorBindingUpdateUnusedWhilePending      =   VK_TRUE;
    features12.descriptorBindingSampledImageUpdateAfterBind   =   VK_TRUE;
    features12.descriptorBindingStorageBufferUpdateAfterBind  =   VK_TRUE;
    features12.shaderSampledImageArrayNonUniformIndexing      =   VK_TRUE;
    features12.shaderStorageBufferArrayNonUniformIndexing     =   VK_TRUE;
    features12.descriptorBindingUniformBufferUpdateAfterBind  =   VK_TRUE;


    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.synchronization2 = VK_TRUE;
    features13.maintenance4     = VK_TRUE;
    features13.pNext = &features12;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType                    =   VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount     =   queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos        =   queueCreateInfos.data();
    deviceCreateInfo.pNext                    =   &features13;
    deviceCreateInfo.enabledExtensionCount    =   static_cast<uint32_t>(requiredDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames  =   requiredDeviceExtensions.data();

    if (vkCreateDevice(physicalDevice,&deviceCreateInfo,nullptr,&device) != VK_SUCCESS)
    {
      throw std::runtime_error("\nCan't create logical device from physical device!");
    }

    // Now retrieve  queues

    if (graphicsFamilyIndex!=-1)
      vkGetDeviceQueue(device,graphicsFamilyIndex,0,&graphicsQueue); // index 0 as we asked for 1 queue per queue family

    if (transferFamilyIndex!=-1)
      vkGetDeviceQueue(device,transferFamilyIndex,0,&transferQueue);
    else
      transferFamilyIndex = graphicsFamilyIndex; // mobile or integrated gpu's only expose one family queue

    if (computeFamilyIndex!=-1)
      vkGetDeviceQueue(device,computeFamilyIndex,0,&computeQueue);
    else
      computeFamilyIndex = graphicsFamilyIndex;

}

void Context::CreateGlobalAllocator()
{
    VmaAllocatorCreateInfo allocatorCreateInfo{};
    //VmaVulkanFunctions functions{};
    //functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    //functions.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

    allocatorCreateInfo.device          =  device;
    allocatorCreateInfo.instance        =  instance;
    allocatorCreateInfo.physicalDevice  =  physicalDevice;
    //allocatorCreateInfo.pVulkanFunctions = &functions;

    if (vmaCreateAllocator(&allocatorCreateInfo, &allocator) != VK_SUCCESS)
    {
      throw std::runtime_error("\nCan't create VMA global allocator!");
    }

}


#pragma endregion



#pragma region Cleanup



Context::~Context()
{

#ifndef NDEBUG
    if (enableValidationLayers)
      DestroyDebugUtilsMessengerEXT(instance,debugMessenger,nullptr);
#endif

  vmaDestroyAllocator(allocator);
  vkDestroyDevice(device,nullptr);
  vkDestroySurfaceKHR(instance,surface,nullptr);
  vkDestroyInstance(instance,nullptr);

}




#pragma endregion