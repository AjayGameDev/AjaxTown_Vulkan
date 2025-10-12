#include "Framebuffer.h"

#include "stdexcept"
#include "vma/vk_mem_alloc.h"

Framebuffer::Framebuffer(VulkanContext &context, Swapchain &swapchain,ImageManager& imageManager,Renderpass& renderpass) : context(context),swapchain(&swapchain),imageManager(&imageManager),renderpass(&renderpass)
{
    CreateImages();
    CreateFrameBuffer();
}

void Framebuffer::CreateImages()
{

    imageManager->Create2DImage(diffuse,context.surfaceCapabilities.currentExtent.width,context.surfaceCapabilities.currentExtent.height,diffuseFormat,VMA_MEMORY_USAGE_GPU_ONLY,true);
    imageManager->Create2DImage(normal,context.surfaceCapabilities.currentExtent.width,context.surfaceCapabilities.currentExtent.height,normalFormat,VMA_MEMORY_USAGE_GPU_ONLY,true);
    imageManager->Create2DImage(RMAO,context.surfaceCapabilities.currentExtent.width,context.surfaceCapabilities.currentExtent.height,RMAOFormat,VMA_MEMORY_USAGE_GPU_ONLY,true);
    imageManager->Create2DImageDepth(depth,context.surfaceCapabilities.currentExtent.width,context.surfaceCapabilities.currentExtent.height,depthFormat,VMA_MEMORY_USAGE_GPU_ONLY);
}

void Framebuffer::CreateFrameBuffer()
{
    // Make sure it's in the same order as the renderpass otherwise it will throw validation error
    std::vector<VkImageView> attachments = { diffuse.imageView ,normal.imageView,RMAO.imageView,depth.imageView };

    for (auto swapchainImageView : swapchain->swapchainImageViews)
    {
        attachments.push_back(swapchainImageView);

        VkFramebuffer framebuffer;
        VkFramebufferCreateInfo framebufferCreateInfo{};

        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.width = context.surfaceCapabilities.currentExtent.width;
        framebufferCreateInfo.height = context.surfaceCapabilities.currentExtent.height;
        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.layers = 1;
        framebufferCreateInfo.renderPass = renderpass->renderpass;


        if (vkCreateFramebuffer(context.device,&framebufferCreateInfo,nullptr,&framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Can't create framebuffer");
        }

        framebuffers.push_back(framebuffer);
        attachments.pop_back();
    }
}

Framebuffer::~Framebuffer()
{
    for (auto framebuffer: framebuffers)
    {
        vkDestroyFramebuffer(context.device,framebuffer,nullptr);
    }
}
