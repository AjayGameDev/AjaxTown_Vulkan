#include "Headers.h"

// abstracted in  context
#pragma region Debugging callback

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

// abstracted in shader class
#pragma region Shader helper functions

enum class ShaderType : uint8_t
{
    vert,
    frag,
    comp
};

std::vector<char> ReadShader(const std::string& shaderName, const ShaderType shaderType)
{

  std::string shaderTypeName;

  switch (shaderType)
  {

    case ShaderType::vert     :   shaderTypeName  =  ".vert.spv";    break;     // these extensions are not enforced by the vulkan but
    case ShaderType::frag     :   shaderTypeName  =  ".frag.spv";    break;    //  tools like glslc (from shaderc compiler) uses these extensions
    case ShaderType::comp     :   shaderTypeName  =  ".comp.spv";    break;   //   these can be overriden while compiling like using .vertex instead of .vert
    default                   :   shaderTypeName  =  "";             break;  //    but then you have to override it for all (~14) extensions

  }

  std::string filePath = R"(C:\Users\dubey\CLionProjects\AjaxTown\assets\shaders\compiled\)" + shaderName + shaderTypeName;

  std::ifstream file(filePath,std::ios::binary | std::ios::ate);
  if (!file.is_open())
  {
    std::cout << "\nCan't open shader file!" << shaderName;
  }
  const long long fileSize = file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(),fileSize);
  file.close();

  return buffer;

}

VkShaderModule CreateShaderModule(const VkDevice& device,const std::vector<char>& shaderCode)
{

  VkShaderModuleCreateInfo shaderModuleCreateInfo{};

  shaderModuleCreateInfo.sType     =  VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleCreateInfo.codeSize  =  shaderCode.size();
  shaderModuleCreateInfo.pCode     =  reinterpret_cast<const uint32_t*>(shaderCode.data());

  VkShaderModule shaderModule;

  if (vkCreateShaderModule(device,&shaderModuleCreateInfo,nullptr,&shaderModule) != VK_SUCCESS)
    std::cout << "\nCan't create shader module!";

  return shaderModule;

}

#pragma endregion

// abstracted in buffer manager
#pragma region Buffer helper functions

VkCommandBuffer BeginOneTimeCommandBuffer(VkDevice device, VkCommandPool commandPool)
{
  VkCommandBuffer commandBuffer = nullptr;
  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandBufferCount = 1;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  vkAllocateCommandBuffers(device,&commandBufferAllocateInfo,&commandBuffer);

  VkCommandBufferBeginInfo commandBufferBeginInfo{};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer,&commandBufferBeginInfo);

  return commandBuffer;

}


void EndOneTimeCommandBuffer( VkCommandBuffer commandBuffer, VkQueue queue, VkDevice device, VkCommandPool commandPool)
{
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(queue,1,&submitInfo,VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(device,commandPool,1,&commandBuffer);
}

#pragma endregion


int main(int argc, char* argv[])
{
  std::vector<Vertex_Standard> vertices;
/*
  =
  {

     {  { 0.0,-0.5 },{ 1.0,0.0,0.0 } },

     {  { 0.0,-0.5 },{ 0.0,1.0,0.0 } },

     {  { 0.0,-0.5 },{ 0.0,0.0,1.0 } }

  };
*/

  // abstracted in  context
#pragma region Platform-Architecture specific setup

#ifdef NDEBUG

  //std::cout << "\n" << "Not Debug Mode "; // Disable validation layers for better optimization
  const bool enableValidationLayers = false;

#else

  std::cout << "\n" << "Debug Mode";
  constexpr bool enableValidationLayers = true;

#endif

#pragma endregion

  // abstracted in window
#pragma region SDL window creation with vulkan support
  //---------------------------------------------------------------------------- Step I : Create a window with sdl3, which supports vulkan
  Window window("Ajax Town",640,480);

#pragma endregion

  // abstracted in  context
#pragma region Vulkan Instance Creation
  //---------------------------------------------------------------------------- Step II : Create a Vulkan instance


  //---------------------------------------------------------------------------- Describe your app info

  VkApplicationInfo appInfo{};

  appInfo.sType                =   VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName     =   "Ajax Town";
  appInfo.applicationVersion   =   1.0;
  appInfo.pEngineName          =   "Ajax";
  appInfo.engineVersion        =   1.0;
  appInfo.apiVersion           =   VK_MAKE_API_VERSION(0,1,1,0);  // Vulkan version 1.1 supports most of the android (variant,major,minor,patch)

  //---------------------------------------------------------------------------- Get all available extensions

  uint32_t extensionsCount;
  const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
  char const* const* extensions = window.GetExtensions(extensionsCount);
  std::vector<const char*> extensionsVector(extensions,extensions+extensionsCount);

  if (enableValidationLayers)
    extensionsVector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  if (extensionsVector.empty())
  {
    std::cout << "Can't load any vulkan extension! \n";
    return -1;
  }

  //---------------------------------------------------------------------------- Vulkan instance creation info

  VkInstanceCreateInfo createInfo = {};
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

  createInfo.sType                     =   VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo          =   &appInfo;
  createInfo.enabledExtensionCount     =   extensionsVector.size();
  createInfo.ppEnabledExtensionNames   =   extensionsVector.data();

#ifndef NDEBUG
  if (enableValidationLayers)
  {
    createInfo.enabledLayerCount         =   validationLayers.size();
    createInfo.ppEnabledLayerNames       =   validationLayers.empty() ? nullptr : validationLayers.data();


    debugCreateInfo.sType            =  VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity  =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debugCreateInfo.messageType      =  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback  =  DebugCallback;
    debugCreateInfo.pUserData        =  nullptr;

    createInfo.pNext                 =  &debugCreateInfo;
  }
#else
  createInfo.pNext = nullptr;
#endif


  //---------------------------------------------------------------------------- Vulkan instance creation

  VkInstance instance = nullptr;
  if (vkCreateInstance(&createInfo,nullptr,&instance) != VK_SUCCESS)
  {
    std::cout << "\nCan't create vulkan instance ";
    return -1;
  }
  VkDebugUtilsMessengerEXT debugMessenger;
  if (enableValidationLayers)
  {
    // Create a message only if validation layers are enabled
#ifndef NDEBUG
    if (CreateDebugUtilsMessengerEXT(instance,&debugCreateInfo,nullptr,&debugMessenger) != VK_SUCCESS)
      std::cout << "\nCan't create debug messenger";
#endif

  }

#pragma endregion

  // abstracted in  context
#pragma region Vulkan Surface Creation

  VkSurfaceKHR vkSurface;
  if (!SDL_Vulkan_CreateSurface(window.window,instance,nullptr,&vkSurface))
  {
    std::cout << "\nCan't create SDL surface!";
    return -1;
  }

#pragma endregion

  // abstracted in  context
#pragma region Selecting Physical Device

  //---------------------------------------------------------------------------- Step III : Get physical devices

  VkPhysicalDevice physicalDevice = nullptr;
  uint32_t deviceCount = 0;

  vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr); // query how many physical devices are present
  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  vkEnumeratePhysicalDevices(instance,&deviceCount,physicalDevices.data()); // Get all physical devices

  for (const auto device : physicalDevices)
  {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device,&deviceProperties);


    VkSampleCountFlags sampleCountFlag = deviceProperties.limits.framebufferColorSampleCounts;
    if (sampleCountFlag & VK_SAMPLE_COUNT_32_BIT)
      std::cout << "\n" << "32 Sample count supported";
    else if (sampleCountFlag & VK_SAMPLE_COUNT_16_BIT)
      std::cout << "\n" << "16 Sample count supported";
    else if (sampleCountFlag & VK_SAMPLE_COUNT_8_BIT)
      std::cout << "\n" << "8 Sample count supported";
    else if (sampleCountFlag & VK_SAMPLE_COUNT_4_BIT)
      std::cout << "\n" << "4 Sample count supported";

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device,&deviceFeatures);

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
    std::cout << "\nNo physical device detected with vulkan support!";
  }



#pragma endregion

  // abstracted in  context
#pragma region Checking physical device for required queues

  // After picking the physical device, we need to check if it supports the queue that we want to use it for
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,&queueFamilyCount,queueFamilyProperties.data());

  int selectedQueueFamily = -1;
  for (int i = 0; i < queueFamilyCount; ++i)
  {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      selectedQueueFamily = i;
      break;
    }
  }

  // Checking for presentation support
  VkBool32 presentationSupport = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,selectedQueueFamily,vkSurface,&presentationSupport);
  if (!presentationSupport)
  {
    std::cout << "\nSelected physical device can't present to the surface!";
  }


#pragma endregion

  // abstracted in  context
#pragma region Creating logical device and required queues

  // After selecting the physical device and getting graphics queue we can create a logical device

  VkDevice device = nullptr;
  float queuePriority = 1.0f;

  //const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME }; // it's not part of vulkan core so have to enable the swap-chain as an extension
  const std::vector<const char*> requiredDeviceExtensions = {  VK_KHR_SWAPCHAIN_EXTENSION_NAME  }; // it's not part of vulkan core, so have to enable the swap-chain as an extension

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType             =  VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex  =  selectedQueueFamily;
  queueCreateInfo.queueCount        =  1;
  queueCreateInfo.pQueuePriorities  =  &queuePriority;

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType                    =   VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount     =   1;
  deviceCreateInfo.pQueueCreateInfos        =   &queueCreateInfo;
  deviceCreateInfo.pNext                    =   nullptr;
  deviceCreateInfo.enabledExtensionCount    =   static_cast<uint32_t>(requiredDeviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames  =   requiredDeviceExtensions.data();

  if (vkCreateDevice(physicalDevice,&deviceCreateInfo,nullptr,&device) != VK_SUCCESS)
  {
    std::cout << "\nCan't create logical device from physical device!";
  }
  // Now creating graphics queue
  VkQueue graphicsQueue;
  vkGetDeviceQueue(device,selectedQueueFamily,0,&graphicsQueue);

#pragma endregion

  // abstracted in  context
#pragma region Query Swapchain support, surface formats and present modes



  // Query surface capabilities
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,vkSurface,&surfaceCapabilities);

  // Query surface formats
  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,vkSurface,&surfaceFormatCount,nullptr);
  std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,vkSurface,&surfaceFormatCount,surfaceFormats.data());

  // Query present modes
  uint32_t presentModesCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,vkSurface,&presentModesCount,nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModesCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,vkSurface,&presentModesCount,presentModes.data());

  // Selecting surface format
  VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
  for (auto format : surfaceFormats)
  {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      surfaceFormat = format;
      break;
    }
  }

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // Present as fast as possible | tearing-high chance | latency-low
  for (auto mode : presentModes)
  {
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) // triple buffering | tearing-low chance | latency-bit higher
    {
      presentMode = mode;
      break;
    }

  }


  #pragma endregion

  // abstracted in  swapchain
#pragma region Creating swapchain and swapchain images with imageviews for each swapchain image

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};
  swapChainCreateInfo.sType               =     VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface             =     vkSurface;
  swapChainCreateInfo.minImageCount       =     surfaceCapabilities.minImageCount + 1;
  swapChainCreateInfo.imageFormat         =     surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace     =     surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent         =     surfaceCapabilities.currentExtent;
  swapChainCreateInfo.imageArrayLayers    =     1;
  swapChainCreateInfo.imageUsage          =     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapChainCreateInfo.imageSharingMode    =     VK_SHARING_MODE_EXCLUSIVE;
  swapChainCreateInfo.preTransform        =     surfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha      =     VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode         =     presentMode;
  swapChainCreateInfo.clipped             =     VK_TRUE;
  swapChainCreateInfo.oldSwapchain        =     VK_NULL_HANDLE;


  VkSwapchainKHR swapChain = nullptr;
  if (vkCreateSwapchainKHR(device,&swapChainCreateInfo,nullptr,&swapChain) != VK_SUCCESS)
  {
    std::cout << "\nCan't create swapchain!";
    return -1;
  }




  // Get swapchain images (2 for double buffering, 3 for triple buffering)
  // and create image views for them (these are wrappers around images with more info like format, mip-level,depth,image type like 1D,2D,3D)

  uint32_t swapchainImageCount = 0;
  vkGetSwapchainImagesKHR(device,swapChain,&swapchainImageCount,nullptr);
  std::vector<VkImage> swapchainImages(swapchainImageCount);
  vkGetSwapchainImagesKHR(device,swapChain,&swapchainImageCount,swapchainImages.data());

  std::vector<VkImageView> swapchainImageViews(swapchainImageCount);

  for (int i = 0; i < swapchainImageCount; ++i)
  {
    VkImageViewCreateInfo imageViewCreateInfo {};

    imageViewCreateInfo.sType                             =     VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.format                            =     surfaceFormat.format;
    imageViewCreateInfo.image                             =     swapchainImages[i];
    imageViewCreateInfo.viewType                          =     VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.components.r                      =     VK_COMPONENT_SWIZZLE_IDENTITY;    //  Don't remap anything just use r component in image as it is
    imageViewCreateInfo.components.g                      =     VK_COMPONENT_SWIZZLE_IDENTITY;   //   Don't remap anything just use g component in image as it is
    imageViewCreateInfo.components.b                      =     VK_COMPONENT_SWIZZLE_IDENTITY;  //    Don't remap anything just use b component in image as it is
    imageViewCreateInfo.components.a                      =     VK_COMPONENT_SWIZZLE_IDENTITY; //     Don't remap anything just use a component in image as it is
    imageViewCreateInfo.subresourceRange.aspectMask       =     VK_IMAGE_ASPECT_COLOR_BIT;    //      Image view will give access to the color part of the image, use VK_IMAGE_ASPECT_DEPTH_BIT for depth image, VK_IMAGE_ASPECT_STENCIL_BIT for stencil image
    imageViewCreateInfo.subresourceRange.baseMipLevel     =     0;                           //       Start from first mip level
    imageViewCreateInfo.subresourceRange.levelCount       =     1;                          //        Just use 1 mip level, swap chain images don't use mip-maps anyway
    imageViewCreateInfo.subresourceRange.layerCount       =     1;                         //         Images can be array like 2d array, cubemap array so just use 1 array layer
    imageViewCreateInfo.subresourceRange.baseArrayLayer   =     0;                        //          This is layer 0 ie first layer


    if (vkCreateImageView(device,&imageViewCreateInfo,nullptr,&swapchainImageViews[i]) != VK_SUCCESS)
    {
      std::cout << "\nCan't create image views for swapchain images!";
      return -1;
    }
  }


#pragma endregion

  // abstracted in  renderpass
#pragma region Creating Render pass


  VkAttachmentDescription colorAttachment{};
  colorAttachment.format          =  surfaceFormat.format;
  colorAttachment.samples         =  VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp         =  VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout     =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.layout      =  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachmentRef.attachment  =  0;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint     =   VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount  =   1;
  subpass.pColorAttachments     =   &colorAttachmentRef;


  VkRenderPassCreateInfo renderpassCreateInfo{};
  renderpassCreateInfo.sType              =     VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderpassCreateInfo.attachmentCount    =     1;
  renderpassCreateInfo.pAttachments       =     &colorAttachment;
  renderpassCreateInfo.subpassCount       =     1;
  renderpassCreateInfo.pSubpasses         =     &subpass;



  VkRenderPass renderpass = nullptr;

  if (vkCreateRenderPass(device,&renderpassCreateInfo,nullptr,&renderpass) != VK_SUCCESS)
  {
    std::cout << "\nCan't create render pass!";
    return -1;
  }

#pragma endregion

  // abstracted in  frame buffer
#pragma region Creating framebuffer for each swapchain image view

  //ImageManager imageManager(context);
  //Framebuffer framebuffer(context,swapChain,imageManager,renderpass);
  std::vector<VkFramebuffer> framebuffers;

  for (auto& swapchainImageView : swapchainImageViews)
  {
    std::vector<VkImageView> attachments = { swapchainImageView };

    VkFramebufferCreateInfo framebufferCreateInfo{};

    framebufferCreateInfo.sType             =     VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass        =     renderpass;
    framebufferCreateInfo.attachmentCount   =     attachments.size();
    framebufferCreateInfo.pAttachments      =     attachments.data();
    framebufferCreateInfo.layers            =     1;
    framebufferCreateInfo.width             =     surfaceCapabilities.currentExtent.width;
    framebufferCreateInfo.height            =     surfaceCapabilities.currentExtent.height;

    VkFramebuffer framebuffer = nullptr;
    if (vkCreateFramebuffer(device,&framebufferCreateInfo,nullptr,&framebuffer) != VK_SUCCESS)
    {
        std::cout << "\nCan't create framebuffer!";
        return -1;
    }

    framebuffers.push_back(framebuffer);

  }
#pragma endregion

// abstracted
#pragma region Creating Semaphore and Fence

  const uint32_t maxFramesInFlight = swapchainImageCount; // 3 for triple buffering (mailbox) and 2 for double buffering (FIFO)

  //---------------------------------------------------------------------  abstracted version

  //FrameSyncManager frameSyncManager(maxFramesInFlight);
  //auto& currentFrameSync = frameSyncManager.GetCurrentFrameSync();
  //currentFrameSync.frameFence.WaitAndReset();
  //currentFrameSync.imageAvailableSemaphore.GetHandle();
  //currentFrameSync.renderFinishedSemaphore.GetHandle();
  //frameSyncManager.NextFrame(); Use it in while loop to advance the frame

  // ------------------------------------------------------------------------------------

  // Semaphores are for GPU-GPU synchronization
  // Fences are for CPU-GPU or GPU-CPU synchornization | vkWaitForFence can be used on cpu side to wait for gpu to finish rendering | vkResetFences to reset fence

  std::vector<VkSemaphore> imageAvailableSemaphore(maxFramesInFlight);   // used to signal that image from swapchain is ready to be rendered to
  std::vector<VkSemaphore> renderFinishedSemaphore(maxFramesInFlight);  //  used to signal that rendering has finished and image can be used for presentation
  std::vector<VkFence>     inFlightFence(maxFramesInFlight);           //   used to wait on cpu side for GPU to finish rendering

  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (int i = 0; i < maxFramesInFlight; ++i)
  {
    vkCreateSemaphore(device,&semaphoreCreateInfo,nullptr,&imageAvailableSemaphore[i]);
    vkCreateSemaphore(device,&semaphoreCreateInfo,nullptr,&renderFinishedSemaphore[i]);
  }


  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < maxFramesInFlight; ++i)
  {
    vkCreateFence(device,&fenceCreateInfo,nullptr,&inFlightFence[i]);
  }

#pragma endregion

  // can be abstracted using shader class
#pragma region Creating shader modules

  auto vertexShaderCode   =  ReadShader("Triangle",ShaderType::vert);
  auto fragmentShaderCode =  ReadShader("Triangle",ShaderType::frag);

  VkShaderModule vertexShaderModule   = CreateShaderModule(device,vertexShaderCode);
  VkShaderModule fragmentShaderModule = CreateShaderModule(device,fragmentShaderCode);
  //Shader triangleShader("Triangle",context);

#pragma endregion


  // abstracted in  Graphics Pipeline
#pragma region Creating graphics pipeline in 8 steps

  // needs to implement reflection but abstraction is kind of done
  #pragma region Creating graphics pipeline layout

  VkPipelineLayout pipelineLayout;

  VkPipelineLayoutCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType                    =   VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineCreateInfo.pushConstantRangeCount   =   0; // Not using push constants
  pipelineCreateInfo.pPushConstantRanges      =   nullptr;
  pipelineCreateInfo.setLayoutCount           =   0; // Not using descriptor sets
  pipelineCreateInfo.pSetLayouts              =   nullptr;

  if (vkCreatePipelineLayout(device,&pipelineCreateInfo,nullptr,&pipelineLayout) != VK_SUCCESS)
  {
    std::cout << "\nCan't create pipeline layout!";
    return -1;
  }

  #pragma endregion

  // done
  #pragma region Shader stage setup

  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

  VkPipelineShaderStageCreateInfo vertexStage{};
  vertexStage.sType    =   VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexStage.stage    =   VK_SHADER_STAGE_VERTEX_BIT;
  vertexStage.module   =   vertexShaderModule;
  vertexStage.pName    =   "main";

  shaderStages.push_back(vertexStage);

  VkPipelineShaderStageCreateInfo fragmentStage{};
  fragmentStage.sType    =   VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentStage.stage    =   VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentStage.module   =   fragmentShaderModule;
  fragmentStage.pName    =   "main";

  shaderStages.push_back(fragmentStage);

  #pragma endregion

  // done
  #pragma region Vertex Input State

  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding   = 0;
  bindingDescription.stride    = sizeof(Vertex_Standard);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> attributeDescritions;

  VkVertexInputAttributeDescription positionAttribute
  {
      .binding  = 0,
      .location = 0,
      .format   = VK_FORMAT_R32G32B32_SFLOAT,
      .offset   = offsetof(Vertex_Standard,position)
  };
  attributeDescritions.push_back(positionAttribute);

  VkVertexInputAttributeDescription normalAttribute
  {
    .binding  = 0,
    .location = 1,
    .format   = VK_FORMAT_R32G32B32_SFLOAT,
    .offset   = offsetof(Vertex_Standard,normal)
  };
  attributeDescritions.push_back(normalAttribute);

  VkVertexInputAttributeDescription uvAttribute
  {
    .binding  = 0,
    .location = 2,
    .format   = VK_FORMAT_R32G32_SFLOAT,
    .offset   = offsetof(Vertex_Standard,uv)
  };
  attributeDescritions.push_back(uvAttribute);

  VkVertexInputAttributeDescription tangentAttribute
  {
    .binding  = 0,
    .location = 3,
    .format   = VK_FORMAT_R32G32B32_SFLOAT,
    .offset   = offsetof(Vertex_Standard,tangent)
  };
  attributeDescritions.push_back(tangentAttribute);

  /*

  VkVertexInputAttributeDescription bitangentAttribute
   {
     .binding  = 0,
     .location = 4,
     .format   = VK_FORMAT_R32G32B32_SFLOAT,
     .offset   = offsetof(Vertex_Standard,bitangent)
   };
  attributeDescritions.push_back(bitangentAttribute);

  */

  VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
  vertexInputStateInfo.sType                             =   VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateInfo.vertexBindingDescriptionCount     =   1; // No vertex binding
  vertexInputStateInfo.pVertexBindingDescriptions        =   &bindingDescription;
  vertexInputStateInfo.vertexAttributeDescriptionCount   =   attributeDescritions.size(); // No vertex attributes
  vertexInputStateInfo.pVertexAttributeDescriptions      =   attributeDescritions.data();

  #pragma endregion

  // done
  #pragma region Input Assembly

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
  inputAssemblyInfo.sType                     =   VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyInfo.topology                  =   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyInfo.primitiveRestartEnable    =   VK_FALSE;

  #pragma endregion

  // done
  #pragma region Viewport and Scissor

  VkPipelineViewportStateCreateInfo viewportInfo{};
  viewportInfo.sType           =   VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount   =   1;
  viewportInfo.pViewports      =   nullptr;
  viewportInfo.scissorCount    =   1;
  viewportInfo.pScissors       =   nullptr;

  #pragma endregion

  // done
  #pragma region Dynamic States

  std::vector<VkDynamicState> dynamicStates =
  {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
  dynamicStateInfo.sType              =   VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateInfo.dynamicStateCount  =   dynamicStates.size();
  dynamicStateInfo.pDynamicStates     =   dynamicStates.data();

#pragma endregion

  // done
  #pragma region Rasterizer

  VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
  rasterizerInfo.sType                      =     VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerInfo.cullMode                   =     VK_CULL_MODE_BACK_BIT;    // cull back faces, none for grass or double sided materials
  rasterizerInfo.depthClampEnable           =     VK_FALSE;                // mostly used for shadow stage
  rasterizerInfo.depthBiasEnable            =     VK_FALSE;               // mostly used for shadow maps or decals to avoid z fighting
  rasterizerInfo.rasterizerDiscardEnable    =     VK_FALSE;              // for compute stage we can completely discard rasterization
  rasterizerInfo.polygonMode                =     VK_POLYGON_MODE_FILL; // use line mode for wireframe debugging
  rasterizerInfo.lineWidth                  =     1.0f;
  rasterizerInfo.frontFace                  =     VK_FRONT_FACE_CLOCKWISE;

  #pragma endregion

  // done kine of
  #pragma region Multi-sampling

  // Not using multi-sampling, implementing MSAA,FXAA,SMAA,TAA in shaders makes more sense
  VkPipelineMultisampleStateCreateInfo multisamplingInfo{}; // Initialize with zeroes to avoid invalid values
  multisamplingInfo.sType                  =   VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisamplingInfo.sampleShadingEnable    =   VK_FALSE;
  multisamplingInfo.rasterizationSamples   =   VK_SAMPLE_COUNT_1_BIT;

  #pragma endregion

  // done
  #pragma region Color blend

  VkPipelineColorBlendAttachmentState colorAttachmentInfo{};
  colorAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorAttachmentInfo.blendEnable    = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorStateInfo{};
  colorStateInfo.sType             =   VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorStateInfo.logicOpEnable     =   VK_FALSE;
  colorStateInfo.attachmentCount   =   1; // hardcoding number is not a good practice [improved in abstraction]
  colorStateInfo.pAttachments      =   &colorAttachmentInfo;


  #pragma endregion







  #pragma region Graphics pipeline Info

  VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
  graphicsPipelineInfo.sType                =   VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineInfo.stageCount           =   shaderStages.size();
  graphicsPipelineInfo.pStages              =   shaderStages.data();
  graphicsPipelineInfo.pVertexInputState    =   &vertexInputStateInfo;
  graphicsPipelineInfo.pInputAssemblyState  =   &inputAssemblyInfo;
  graphicsPipelineInfo.pViewportState       =   &viewportInfo;
  graphicsPipelineInfo.pRasterizationState  =   &rasterizerInfo;
  graphicsPipelineInfo.pMultisampleState    =   &multisamplingInfo;
  graphicsPipelineInfo.pDynamicState        =   &dynamicStateInfo;
  graphicsPipelineInfo.pColorBlendState     =   &colorStateInfo;
  graphicsPipelineInfo.layout               =   pipelineLayout;
  graphicsPipelineInfo.renderPass           =   renderpass;
  graphicsPipelineInfo.subpass              =   0;
  graphicsPipelineInfo.basePipelineHandle   =   VK_NULL_HANDLE;
  graphicsPipelineInfo.pNext                =   nullptr;

  #pragma endregion

  #pragma region Finally creating the graphics pipeline

  VkPipeline graphicsPipeline = nullptr;
  if (vkCreateGraphicsPipelines(device,nullptr,1,&graphicsPipelineInfo,nullptr,&graphicsPipeline) != VK_SUCCESS)
  {
    std::cout << "\nCan't create graphics pipeline!";
  }

  #pragma endregion

#pragma endregion

// abstracted in command buffer
#pragma region Dynamic viewport and scissor

  // Set dynamic viewport
  VkViewport viewport{};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width  = static_cast<float>(swapChainCreateInfo.imageExtent.width);
  viewport.height = static_cast<float>(swapChainCreateInfo.imageExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;


  // Set dynamic Scissor
  VkRect2D scissor{};
  scissor.offset    =   {0,0};
  scissor.extent    =   swapChainCreateInfo.imageExtent;

#pragma endregion

// abstracted in command buffer and command pool
#pragma region Allocating command Buffer from command pool

  //  Create command buffer
  std::vector<VkCommandBuffer> commandBuffer(swapchainImageCount);
  //std::vector<VkCommandPool> commandPool(swapchainImageCount);
  VkCommandPool commandPool;

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = selectedQueueFamily;

  // Only one command pool is created per queue family

  /*
  for (int i = 0; i < swapchainImageCount; ++i)
  {
    if (vkCreateCommandPool(device,&commandPoolCreateInfo,nullptr,&commandPool[i]) !=  VK_SUCCESS)
    {
      std::cout << "\nCan't create command pool!";
      return -1;
    }
  }
*/

  if (vkCreateCommandPool(device,&commandPoolCreateInfo,nullptr,&commandPool) !=  VK_SUCCESS)
  {
    std::cout << "\nCan't create command pool!";
    return -1;
  }


  for (int i = 0; i < swapchainImageCount; ++i)
  {
    // Allocate command buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType                 =    VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    //commandBufferAllocateInfo.commandPool         =    commandPool[i];
    commandBufferAllocateInfo.commandPool           =    commandPool;
    commandBufferAllocateInfo.level                 =    VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount    =    1;

    if (vkAllocateCommandBuffers(device,&commandBufferAllocateInfo,&commandBuffer[i]) != VK_SUCCESS)
    {
      std::cout << "\nCan't allocate command buffer!";
      return -1;
    }
  }

  VkClearValue clearColor = {{ .1f,.2f,.3f,1.0f } };


#pragma endregion

  // can be abstracted using buffer manager
#pragma region Creating global VMA allocator
/*
  VmaAllocatorCreateInfo allocatorCreateInfo{};
  allocatorCreateInfo.instance         =   instance;
  allocatorCreateInfo.device           =   device;
  allocatorCreateInfo.physicalDevice   =   physicalDevice;

  VmaAllocator allocator;

  if (vmaCreateAllocator(&allocatorCreateInfo,&allocator) != VK_SUCCESS)
  {
    std::cout << "\nCan't create global VMA allocator!";
    return -1;
  }*/
  //BufferManager bufferManager(device,physicalDevice,instance,selectedQueueFamily,graphicsQueue);

#pragma endregion

  // can be abstracted using buffer
#pragma region Creating Staging Buffer
/*
  VkBuffer stagingBuffer;
  VmaAllocation stagingAllocation;

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.size = sizeof(Vertex) * vertices.size();

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  if (vmaCreateBuffer(allocator,&bufferCreateInfo,&allocationCreateInfo,&stagingBuffer,&stagingAllocation,nullptr) != VK_SUCCESS)
  {
    std::cout << "\nCan't create staging buffer!";
    return -1;
  }
  */
  //const size_t vertexBufferSize = sizeof(Vertex) * vertices.size();
  //Buffer stagingBuffer = bufferManager.CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);

#pragma endregion

  // can be abstracted using buffer
#pragma region copy vertices into staging buffer

  //void* data; // Pointer to reference to the start of the memory
  //vmaMapMemory(allocator,stagingAllocation,&data); // Write address of start of the buffer into this data pointer
  //memcpy(data,vertices.data(),bufferCreateInfo.size); // Copy data to that buffer
  //vmaUnmapMemory(allocator,stagingAllocation); // we are done writing into this buffer, don't use it after as data is invalid now and we need to map it again for use

  //stagingBuffer.CopyData(vertices.data(),sizeof(vertices)); wrong sizeof(vertices) will return size of Vertex object and not data in it so it will give different result than sizeof(Vertex) * vertices.size()

  //stagingBuffer.CopyData(vertices.data(),vertexBufferSize);


#pragma endregion

  // can be abstracted using buffer
#pragma region Creating buffer on gpu
/*
  VkBuffer vertexBuffer;
  VmaAllocation vertexAllocation;

  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  vmaCreateBuffer(allocator,&bufferCreateInfo,&allocationCreateInfo,&vertexBuffer,&vertexAllocation,nullptr);*/
  //float a = stagingBuffer.bufferCreateInfo.size;

  //Buffer vertexBuffer = bufferManager.CreateBuffer(stagingBuffer.bufferCreateInfo.size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);


#pragma endregion

  // can be abstracted using buffer manager
#pragma region copy data from staging buffer to gpu buffer
/*
  VkCommandPool tempCommandPool = nullptr;

  VkCommandPoolCreateInfo tempCommandPoolCreateInfo{};

  tempCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  tempCommandPoolCreateInfo.queueFamilyIndex = selectedQueueFamily;
  tempCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // This command pool is for one time command buffer

  vkCreateCommandPool(device,&tempCommandPoolCreateInfo,nullptr,&tempCommandPool);


  VkCommandBuffer cmd =  BeginOneTimeCommandBuffer(device,tempCommandPool);

  VkBufferCopy bufferCopyRegion{};
  bufferCopyRegion.size = bufferCreateInfo.size;
  vkCmdCopyBuffer(cmd,stagingBuffer,vertexBuffer,1,&bufferCopyRegion);
  EndOneTimeCommandBuffer(cmd,graphicsQueue,device,tempCommandPool);

  vmaDestroyBuffer(allocator,stagingBuffer,stagingAllocation);

*/
  //bufferManager.CopyBuffer(&stagingBuffer,&vertexBuffer);

#pragma endregion

// _____________________________________________________________________ to be done
#pragma region main loop

  bool closeWindow = false;
  GameTime time;
  uint8_t currentFrame = 0; // size [ 1 byte ] range [ 0 - 255 ]

  {
    BufferManager bufferManager(device,physicalDevice,instance,selectedQueueFamily,graphicsQueue);
    const size_t vertexBufferSize = sizeof(Vertex_Standard) * vertices.size();
    Buffer stagingBuffer = bufferManager.CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.CopyData(vertices.data(),vertexBufferSize);
    Buffer vertexBuffer = bufferManager.CreateBuffer(stagingBuffer.bufferCreateInfo.size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    bufferManager.CopyBuffer(&stagingBuffer,&vertexBuffer);


    while(!closeWindow)
    {
      time.Update();
      SDL_Event e;
      while (SDL_PollEvent(&e))
      {
        if (e.type == SDL_EVENT_QUIT)
          closeWindow = true;
      }

#pragma region Acquire image from swapchain before rendering

      vkWaitForFences(device,1,&inFlightFence[currentFrame],VK_TRUE,UINT64_MAX);
      vkResetFences(device,1,&inFlightFence[currentFrame]);

      uint32_t imageIndex; // use this rendering attachment info instead of swapchainImageViews[0]
      vkAcquireNextImageKHR(device,swapChain,UINT64_MAX,imageAvailableSemaphore[currentFrame],nullptr,&imageIndex);

#pragma endregion

#pragma region Record command buffers and submit them to graphics/compute/transfer queue

      VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

      VkSubmitInfo submitInfo{};

      submitInfo.sType                  =    VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount     =    1;
      submitInfo.pWaitSemaphores        =    &imageAvailableSemaphore[currentFrame];
      submitInfo.commandBufferCount     =    1;
      submitInfo.pCommandBuffers        =    &commandBuffer[currentFrame];
      submitInfo.pWaitDstStageMask      =    waitStages;
      submitInfo.signalSemaphoreCount   =    1;
      submitInfo.pSignalSemaphores      =    &renderFinishedSemaphore[currentFrame];


      // Reset command buffer
      vkResetCommandBuffer(commandBuffer[currentFrame],0);

      //  Begin command buffer
      VkCommandBufferBeginInfo commandBufferBeginInfo{};
      commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      if (vkBeginCommandBuffer(commandBuffer[currentFrame],&commandBufferBeginInfo) != VK_SUCCESS)
      {
        std::cout << "\nCan't begin the command buffer!";
        return -1;
      }

      vkCmdSetViewport(commandBuffer[currentFrame],0,1,&viewport);
      vkCmdSetScissor(commandBuffer[currentFrame],0,1,&scissor);

      VkRenderPassBeginInfo renderpassBeginInfo{};
      renderpassBeginInfo.sType                =    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderpassBeginInfo.framebuffer          =    framebuffers[imageIndex];
      renderpassBeginInfo.renderPass           =    renderpass;
      renderpassBeginInfo.clearValueCount      =    1;
      renderpassBeginInfo.pClearValues         =    &clearColor;
      renderpassBeginInfo.renderArea.offset    =    {0,0};
      renderpassBeginInfo.renderArea.extent    =    surfaceCapabilities.currentExtent;

      vkCmdBeginRenderPass(commandBuffer[currentFrame],&renderpassBeginInfo,VK_SUBPASS_CONTENTS_INLINE);


      // Bind pipeline
      vkCmdBindPipeline(commandBuffer[currentFrame],VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline);
      VkDeviceSize offset = 0;
      vkCmdBindVertexBuffers(commandBuffer[currentFrame],0,1,&vertexBuffer.buffer,&offset);

      // Draw command buffer
      //vkCmdDraw(commandBuffer[currentFrame],3,1,0,0);
      vkCmdDraw(commandBuffer[currentFrame],vertices.size(),1,0,0);

      vkCmdEndRenderPass(commandBuffer[currentFrame]);




      if (vkEndCommandBuffer(commandBuffer[currentFrame]) != VK_SUCCESS)
      {
        std::cout << "\nCan't record the command buffer!";
        return -1;
      }


#pragma endregion

#pragma region submit graphics queue

      vkQueueSubmit(graphicsQueue,1,&submitInfo,inFlightFence[currentFrame]);
#pragma endregion

#pragma region Present Image

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType                 =     VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount    =     1;
      presentInfo.pWaitSemaphores       =     &renderFinishedSemaphore[currentFrame];
      presentInfo.swapchainCount        =     1;
      presentInfo.pSwapchains           =     &swapChain;
      presentInfo.pImageIndices         =     &imageIndex;

      vkQueuePresentKHR(graphicsQueue,&presentInfo);

#pragma endregion


      currentFrame = (currentFrame + 1) % maxFramesInFlight; // advance the frame

      //std::cout << "\nFPS " << 1 / time.deltaTime << "  Delta Time " <<  time.deltaTime * 1000 << std::flush;

    }


    std::cout << "\n" << "Welcome to Ajax Town!" << std::endl;

  }
#pragma endregion

  // abstracted in different classes
#pragma region clean-up

  // Clean-up
#ifndef NDEBUG

  if (enableValidationLayers)
    DestroyDebugUtilsMessengerEXT(instance,debugMessenger,nullptr);

#endif
  vkDeviceWaitIdle(device);
  //vmaDestroyBuffer(allocator,vertexBuffer,vertexAllocation);
  //vmaDestroyAllocator(allocator);
  for (auto framebuffer : framebuffers)
  {
    vkDestroyFramebuffer(device,framebuffer,nullptr);
  }
  vkDestroyRenderPass(device,renderpass,nullptr);
  /*
  for (int i = 0; i < swapchainImageCount; ++i)
  {
    vkDestroyCommandPool(device,commandPool[i],nullptr);
  }
  */
  vkDestroyCommandPool(device,commandPool,nullptr);
  vkDestroyPipeline(device,graphicsPipeline,nullptr);
  vkDestroyPipelineLayout(device,pipelineLayout,nullptr);
  vkDestroyShaderModule(device,vertexShaderModule,nullptr);
  vkDestroyShaderModule(device,fragmentShaderModule,nullptr);

  for (int i = 0; i < maxFramesInFlight; ++i)
  {
    vkDestroySemaphore(device,renderFinishedSemaphore[i],nullptr);
    vkDestroySemaphore(device,imageAvailableSemaphore[i],nullptr);
    vkDestroyFence(device,inFlightFence[i],nullptr);
  }


  for (auto swapchainImageView : swapchainImageViews)
  {
    vkDestroyImageView(device,swapchainImageView,nullptr);
  }
  vkDestroySwapchainKHR(device,swapChain,nullptr);
  vkDestroyDevice(device,nullptr);
  vkDestroySurfaceKHR(instance,vkSurface,nullptr);

  vkDestroyInstance(instance,nullptr); // Destroy all childrens of instance before destroying the instance

  //SDL_DestroyWindow(window);
  //SDL_Quit();


  return 0;

#pragma endregion


}

