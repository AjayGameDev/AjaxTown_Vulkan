#include "Swapchain.h"
#include "stdexcept"



Swapchain::Swapchain(VulkanContext& context) : context(context)
{
    CreateSwapchain();
    AcquireSwapchainImages();   // Acquire because we don't own them so don't destroy them either in the destructor
    CreateSwapchainImageViews();
}



void Swapchain::CreateSwapchain()
{
    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType               =     VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface             =     context.surface;
    swapChainCreateInfo.minImageCount       =     context.surfaceCapabilities.minImageCount + 1;
    swapChainCreateInfo.imageFormat         =     context.format.format;
    swapChainCreateInfo.imageColorSpace     =     context.format.colorSpace;
    swapChainCreateInfo.imageExtent         =     context.surfaceCapabilities.currentExtent;
    swapChainCreateInfo.imageArrayLayers    =     1;
    swapChainCreateInfo.imageUsage          =     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode    =     VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.preTransform        =     context.surfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha      =     VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode         =     context.presentMode;
    swapChainCreateInfo.clipped             =     VK_TRUE;
    swapChainCreateInfo.oldSwapchain        =     VK_NULL_HANDLE;


    VkSwapchainKHR swapChain = nullptr;
    if (vkCreateSwapchainKHR(context.device,&swapChainCreateInfo,nullptr,&swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create swapchain!");
    }
}

void Swapchain::AcquireSwapchainImages()
{
    // Get swapchain images (2 for double buffering, 3 for triple buffering)

    swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(context.device,swapchain,&swapchainImageCount,nullptr);
    swapchainImages = std::vector<VkImage>(swapchainImageCount);
    vkGetSwapchainImagesKHR(context.device,swapchain,&swapchainImageCount,swapchainImages.data());
}

void Swapchain::CreateSwapchainImageViews()
{
    // Create image views for them (these are wrappers around images with more info like format, mip-level,depth,image type like 1D,2D,3D)

  swapchainImageViews = std::vector<VkImageView>(swapchainImageCount);

  for (int i = 0; i < swapchainImageCount; ++i)
  {
    VkImageViewCreateInfo imageViewCreateInfo {};

    imageViewCreateInfo.sType                             =     VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.format                            =     context.format.format;
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


    if (vkCreateImageView(context.device,&imageViewCreateInfo,nullptr,&swapchainImageViews[i]) != VK_SUCCESS)
        throw std::runtime_error("Can't create image views for swapchain images!");

  }
}

Swapchain::~Swapchain()
{
    // Destroy image views not images as we don't own them
    for (auto swapchainImageView : swapchainImageViews)
    {
        vkDestroyImageView(context.device,swapchainImageView,nullptr);
    }

    vkDestroySwapchainKHR(context.device,swapchain,nullptr);

}

