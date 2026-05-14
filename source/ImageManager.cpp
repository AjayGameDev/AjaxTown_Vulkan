#include "ImageManager.h"
#include "BufferManager.h"
#include "ktx.h"
#include "ktxvulkan.h"
#include "FileManager.h"

ImageManager::ImageManager(Context& context) : context(context)
{

}

void ImageManager::Create2DImage(Image& image,uint32_t width, uint32_t height, VkFormat format, VmaMemoryUsage vmaMemoryUsage, bool renderTarget,VkSampleCountFlagBits samples,VkImageUsageFlags imageUsageFlags,uint32_t mips)
{
    //image.SetContext(context);

    image.imageInfo.sType           =   VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageInfo.extent          =   { width, height, 1 };
    image.imageInfo.format          =   format;
    image.imageInfo.mipLevels       =   mips;
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
    image.imageViewInfo.subresourceRange.levelCount      =   mips;
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

void ImageManager::UploadImageDataToGPU(Image& image,std::string name,BufferManager& bufferManager)
{
    // load texture from disk

    ktxTexture2* texture;
    std::string path = FileManager::GetLocation(FileManager::Texture) + name + ".ktx2";
    std::vector<uint8_t> buffer = FileManager::LoadBytes_8bit(path);
    KTX_error_code result = ktxTexture2_CreateFromMemory(buffer.data(),buffer.size(),KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,&texture);
    //KTX_error_code result = ktxTexture2_CreateFromNamedFile(path.c_str(),KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,&texture);
    if (result!=KTX_SUCCESS)
    {
        //spdlog::error("[AJAX] Can't load image from disk");
        throw std::runtime_error("Can't load image from disk " + path);
    }

    // Create gpu image based on extracted data

    uint32_t width       =  texture->baseWidth;
    uint32_t height      =  texture->baseHeight;
    uint32_t mipLevels   =  texture->numLevels;
    ktx_size_t dataSize  =  texture->dataSize;
    VkFormat format      =  ktxTexture2_GetVkFormat(texture);

    Create2DImage(image,width,height,format,VMA_MEMORY_USAGE_GPU_ONLY,false,VK_SAMPLE_COUNT_1_BIT,VK_IMAGE_USAGE_TRANSFER_DST_BIT,mipLevels);

    // Create staging buffer

    Buffer stagingBuffer = bufferManager.CreateBuffer(dataSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.CopyData(texture->pData,dataSize);

    // Create copy regions for all mips
    std::vector<VkBufferImageCopy> mipRegions(mipLevels);

    for (uint32_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel)
    {
        ktx_size_t mipOffset;
        ktxTexture_GetImageOffset(ktxTexture(texture),mipLevel,0,0,&mipOffset);

        mipRegions[mipLevel].bufferOffset = mipOffset;
        mipRegions[mipLevel].bufferRowLength = 0;
        mipRegions[mipLevel].bufferImageHeight = 0;
        mipRegions[mipLevel].imageOffset = {0,0,0};
        mipRegions[mipLevel].imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT,mipLevel,0,1};
        mipRegions[mipLevel].imageExtent = { std::max(1u, width >> mipLevel), std::max(1u, height >> mipLevel), 1 };

    }

    // allocate and record command buffer with two pipeline barriers, first for Undefined Image -> Transfer destination and second for Transfer destination -> Shader read only

    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = context.transferFamilyIndex;     // If same as graphics then fine otherwise you have to transfer ownership between queues
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    vkCreateCommandPool(context.device,&poolInfo,nullptr,&commandPool);

    VkCommandBuffer commandBuffer = bufferManager.BeginOneTimeCommandBuffer(commandPool);

    // barrier for undefined to transfer destination

    VkImageMemoryBarrier undefined_to_transfer_dst_barrier{};
    undefined_to_transfer_dst_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    undefined_to_transfer_dst_barrier.image = image.image;
    undefined_to_transfer_dst_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    undefined_to_transfer_dst_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    undefined_to_transfer_dst_barrier.srcAccessMask = 0;
    undefined_to_transfer_dst_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    undefined_to_transfer_dst_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    undefined_to_transfer_dst_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    undefined_to_transfer_dst_barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT,0,mipLevels,0,1 };

    vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,0,0,nullptr,0,nullptr,1,&undefined_to_transfer_dst_barrier);

    // actual copy command
    vkCmdCopyBufferToImage(commandBuffer,stagingBuffer.GetHandle(),image.image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,mipLevels,mipRegions.data());

    // transfer destination to shader read

    VkImageMemoryBarrier transfer_dst_to_shader_read_barrier{};
    transfer_dst_to_shader_read_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    transfer_dst_to_shader_read_barrier.image = image.image;
    transfer_dst_to_shader_read_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    transfer_dst_to_shader_read_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    transfer_dst_to_shader_read_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    transfer_dst_to_shader_read_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    transfer_dst_to_shader_read_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    transfer_dst_to_shader_read_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    transfer_dst_to_shader_read_barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT,0,mipLevels,0,1};

    vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,0,0,nullptr,0,nullptr,1,&transfer_dst_to_shader_read_barrier);

    bufferManager.EndOneTimeCommandBuffer(commandBuffer,context.transferQueue,context.device,commandPool);

    ktxTexture2_Destroy(texture);








}