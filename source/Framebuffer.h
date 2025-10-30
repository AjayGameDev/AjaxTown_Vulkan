#pragma once
#include "Context.h"
#include "Swapchain.h"
#include "ImageManager.h"
#include "Renderpass.h"

class Framebuffer 
{
    Context& context;
    Swapchain& swapchain;
    ImageManager& imageManager;
    Renderpass& renderpass;
    std::vector<VkFramebuffer> framebuffers; // each swapchain will have a framebuffer based on double [FIFO] or triple [Mail] buffering
    Image diffuse,normal,RMAO,depth,hdr;
public:

    Framebuffer(Context& context,Swapchain& swapchain,ImageManager& imageManager,Renderpass& renderpass);
    void CreateImages();
    void CreateFrameBuffer();
    VkFramebuffer& GetCurrentFramebuffer(uint8_t frameIndex) { return framebuffers[frameIndex]; }
    ~Framebuffer();


};
