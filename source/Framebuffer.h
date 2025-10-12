#pragma once
#include "VulkanContext.h"
#include "Swapchain.h"
#include "ImageManager.h"
#include "Renderpass.h"

class Framebuffer 
{
    VulkanContext& context;
    Swapchain* swapchain = nullptr;
    ImageManager* imageManager = nullptr;
    Renderpass* renderpass = nullptr;
    std::vector<VkFramebuffer> framebuffers; // each swapchain will have a framebuffer based on double [FIFO] or triple [Mail] buffering
    Image diffuse,normal,RMAO,depth;
public:

    Framebuffer(VulkanContext& context,Swapchain& swapchain,ImageManager& imageManager,Renderpass* renderpass);
    void CreateImages();
    void CreateFrameBuffer();
    ~Framebuffer();


};
