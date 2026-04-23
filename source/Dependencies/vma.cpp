#define VMA_IMPLEMENTATION
//#define VMA_VULKAN_VERSION 1001000  // Vulkan 1.1 (order matters here)
#ifndef VMA_VULKAN_VERSION
#define VMA_VULKAN_VERSION 1003000  // Vulkan 1.3 (order matters here)
#endif

#include "Dependencies/vk_mem_alloc.h"

// This was causing linker error because vma was using vulkan 1.3 for these cmake options -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28
// android 28 doesn't support vulkan 1.3 btw