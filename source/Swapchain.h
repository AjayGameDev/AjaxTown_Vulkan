#pragma once


class Swapchain 
{
    public:
            Context&                     context;
            VkSwapchainKHR               handle;
            uint32_t                     imageCount;
            std::vector<VkImage>         images;
            std::vector<VkImageView>     imageViews;
            VkSwapchainCreateInfoKHR     swapChainCreateInfo{};


            explicit Swapchain(Context& context);
            VkSwapchainKHR& GetHandle() {    return handle;   }
            void CreateSwapchain();
            void AcquireSwapchainImages();
            void CreateSwapchainImageViews();

            ~Swapchain();


};
