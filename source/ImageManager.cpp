#include "ImageManager.h"


ImageManager::ImageManager(Context& context) : context(context)
{

}

void ImageManager::Create2DImage(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage, bool renderTarget,VkSampleCountFlagBits samples,VkImageUsageFlags imageUsageFlags)
{
    //image.SetContext(context);

    image.imageInfo.sType           =   VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageInfo.extent          =   { width, height, 1};
    image.imageInfo.format          =   format;
    image.imageInfo.mipLevels       =   1;
    image.imageInfo.arrayLayers     =   1;
    image.imageInfo.imageType       =   VK_IMAGE_TYPE_2D; // [1D] for luts [2d] for textures [3d] for volumetric/advanced post processing
    image.imageInfo.samples         =   samples;
    image.imageInfo.tiling          =   VK_IMAGE_TILING_OPTIMAL;
    image.imageInfo.sharingMode     =   VK_SHARING_MODE_EXCLUSIVE; // [exclusive] means only one queue family can access it at a time, use barrier for transferring ownership [VK_SHARING_MODE_CONCURRENT] means it can be shared with other queues that you specify at time of creation, tradeoff is performance for simplicity
    image.imageInfo.initialLayout   =   VK_IMAGE_LAYOUT_UNDEFINED;
    image.imageInfo.usage           =   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Important for frame buffer attachments

    if (samples>VK_SAMPLE_COUNT_1_BIT)
    {
        image.imageInfo.usage      |=   VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    }
    else
    {
        image.imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if (imageUsageFlags!=0)
        image.imageInfo.usage |= imageUsageFlags;

    image.allocationCreateInfo.usage = vmaMemoryUsage;

    if (vmaCreateImage(context.allocator,&image.imageInfo,&image.allocationCreateInfo,&image.image,&image.allocation,nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create 2D image ");
    }

    image.imageViewInfo.sType                            =   VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image.imageViewInfo.image                            =   image.image;
    image.imageViewInfo.viewType                         =   VK_IMAGE_VIEW_TYPE_2D; 
    image.imageViewInfo.format                           =   format; // Must match image format
    image.imageViewInfo.subresourceRange.aspectMask      =   VK_IMAGE_ASPECT_COLOR_BIT;  // or DEPTH_BIT
    image.imageViewInfo.subresourceRange.baseMipLevel    =   0;
    image.imageViewInfo.subresourceRange.levelCount      =   1;
    image.imageViewInfo.subresourceRange.baseArrayLayer  =   0;
    image.imageViewInfo.subresourceRange.layerCount      =   1;

    if (vkCreateImageView(context.device,&image.imageViewInfo,nullptr,&image.imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create 2D image view ");
    }

}

void ImageManager::Create2DImageDepth(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage,VkSampleCountFlagBits samples)
{
    image.imageInfo.sType           =   VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageInfo.extent          =   { width, height, 1};
    image.imageInfo.format          =   format;
    image.imageInfo.mipLevels       =   1;
    image.imageInfo.arrayLayers     =   1;
    image.imageInfo.imageType       =   VK_IMAGE_TYPE_2D;
    image.imageInfo.samples         =   samples;
    image.imageInfo.tiling          =   VK_IMAGE_TILING_OPTIMAL;
    image.imageInfo.sharingMode     =   VK_SHARING_MODE_EXCLUSIVE;
    image.imageInfo.initialLayout   =   VK_IMAGE_LAYOUT_UNDEFINED;
    image.imageInfo.usage           =   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (samples>1)
    {
        image.imageInfo.usage           |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    }
    else
    {
        image.imageInfo.usage           |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }
    image.allocationCreateInfo.usage = vmaMemoryUsage;

    if (vmaCreateImage(context.allocator,&image.imageInfo,&image.allocationCreateInfo,&image.image,&image.allocation,nullptr) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create 2D depth image ");
    }

/*
----------------------------------------------------------------------------    Depth vs Depth-Stencil Formats
Depth-Only Formats:

VK_FORMAT_D16_UNORM  - 16-bit depth
VK_FORMAT_D32_SFLOAT - 32-bit float depth

These only contain depth information (Z-buffer values for depth testing).
Depth-Stencil Formats:

VK_FORMAT_D24_UNORM_S8_UINT - 24-bit depth + 8-bit stencil
VK_FORMAT_D32_SFLOAT_S8_UINT - 32-bit float depth + 8-bit stencil

These contain both depth AND stencil information in the same image.

----------------------------------------------------------------------------
*/
    VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT) // 32 bit for depth 8 bit for stencil
        aspectFlag  |= VK_IMAGE_ASPECT_STENCIL_BIT; // add stencil usage only if it is supported format for that

    image.imageViewInfo.sType                            =   VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image.imageViewInfo.image                            =   image.image;
    image.imageViewInfo.viewType                         =   VK_IMAGE_VIEW_TYPE_2D;
    image.imageViewInfo.format                           =   format; // Must match image format
    image.imageViewInfo.subresourceRange.aspectMask      =   aspectFlag;
    image.imageViewInfo.subresourceRange.baseMipLevel    =   0;
    image.imageViewInfo.subresourceRange.levelCount      =   1;
    image.imageViewInfo.subresourceRange.baseArrayLayer  =   0;
    image.imageViewInfo.subresourceRange.layerCount      =   1;

    if (vkCreateImageView(context.device,&image.imageViewInfo,nullptr,&image.imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Can't create 2D depth image view ");
    }

}