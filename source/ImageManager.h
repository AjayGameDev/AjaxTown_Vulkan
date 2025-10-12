#pragma once
#include "VulkanContext.h"


#define diffuseFormat               VK_FORMAT_R8G8B8_UNORM // Base color in linear space
#define normalFormat                VK_FORMAT_A2R10G10B10_UNORM_PACK32 // 10 bit normal depth with no alpha
#define RMAOFormat                  VK_FORMAT_R8G8B8_UNORM
#define depthFormat                 VK_FORMAT_D32_SFLOAT
#define hdrFormat                   VK_FORMAT_R16G16B16_SFLOAT
#define finalImageFormat            VK_FORMAT_R8G8B8_UNORM // Final output


class Image
{
    private:
            VulkanContext& context;
    public:

            VkImage image = nullptr;
            VkImageView imageView = nullptr;
            VmaAllocation allocation = nullptr;
            VkImageCreateInfo imageInfo = {};
            VkImageViewCreateInfo imageViewInfo = {};
            VmaAllocationCreateInfo allocationCreateInfo = {};
            explicit Image(VulkanContext& context);
            //void SetContext(VulkanContext& context) { (this->context = &context); }
            ~Image()
            {
                if (imageView)
                    vkDestroyImageView(context.device,imageView,nullptr);
                if (image && allocation)
                    vmaDestroyImage(context.allocator,image,allocation);
            }
};


class ImageManager
{
    VulkanContext* context;

    public :
            explicit ImageManager(VulkanContext& context);

            void Create2DImage(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage, bool renderTarget = false);
            void Create2DImageDepth(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage);
};