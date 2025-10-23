#pragma once
#include "vector"
#include "VulkanContext.h"


class Swapchain 
{
    public:
            VulkanContext&               context;
            VkSwapchainKHR               swapchain;
            uint32_t                     swapchainImageCount;
            std::vector<VkImage>         swapchainImages;
            std::vector<VkImageView>     swapchainImageViews;



            explicit Swapchain(VulkanContext& context);
            VkSwapchainKHR GetHandle() {    return swapchain;   }
            void CreateSwapchain();
            void AcquireSwapchainImages();
            void CreateSwapchainImageViews();

            ~Swapchain();


};
