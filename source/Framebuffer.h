#pragma once

#include "Swapchain.h"
#include "ImageManager.h"
#include "Renderpass.h"

class Framebuffer
{
public:
    Context& context;
    Swapchain& swapchain;
    ImageManager& imageManager;
    Renderpass& renderpass;
    RendererType rendererType;
    VkSampleCountFlagBits samples;
    std::vector<VkFramebuffer> framebuffers; // each swapchain will have a framebuffer based on double [FIFO] or triple [Mail] buffering
    Image diffuse,normal,RMAO,depth,hdr; // for deferred rendering
    Image hdrColorImage,resolvedImage,depthImage; // for forward rendering



    void CreateImages();
    void CreateFrameBuffer();
    Framebuffer(Context& context,Swapchain& swapchain,ImageManager& imageManager,Renderpass& renderpass,RendererType rendererType,VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    VkFramebuffer& GetCurrentFramebuffer(uint32_t frameIndex) { return framebuffers[frameIndex]; }
    ~Framebuffer();


};
