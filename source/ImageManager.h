#pragma once


#define diffuseFormat               VK_FORMAT_R8G8B8A8_UNORM // Base color in linear space
#define normalFormat                VK_FORMAT_A2R10G10B10_UNORM_PACK32 // 10 bit normal depth with no alpha
#define RMAOFormat                  VK_FORMAT_R8G8B8A8_UNORM
#define depthFormat                 VK_FORMAT_D32_SFLOAT
#define hdrFormat                   VK_FORMAT_R16G16B16A16_SFLOAT
#define finalImageFormat            VK_FORMAT_B8G8R8A8_SRGB // Final output [Change later TO DO ]


class Image
{
    private:
            Context& context;
    public:

            VkImage image = nullptr;
            VkImageView imageView = nullptr;
            VmaAllocation allocation = nullptr;
            VkImageCreateInfo imageInfo = {};
            VkImageViewCreateInfo imageViewInfo = {};
            VmaAllocationCreateInfo allocationCreateInfo = {};
            explicit Image(Context& context):context(context){};
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
    Context& context;

    public :
            explicit ImageManager(Context& context);

            void Create2DImage(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage, bool renderTarget = false,VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,VkImageUsageFlags imageUsageFlags = 0);
            void Create2DImageDepth(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage,VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
};