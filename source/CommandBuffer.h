#pragma once

#include <vector>

#include "Context.h"


class CommandBuffer 
{
    private:
            VkCommandBuffer handle;
            bool isRecording;

    public:
            CommandBuffer() : handle(VK_NULL_HANDLE),isRecording(false) {};
            explicit CommandBuffer(VkCommandBuffer handle) : handle(handle),isRecording(false) {};
            ~CommandBuffer(){};
            VkCommandBuffer& GetHandle() { return handle; }

#pragma region Life cycle

            void Begin(VkFlags flags = 0)
            {
                VkCommandBufferBeginInfo beginInfo{};

                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = flags;

                vkBeginCommandBuffer(handle,&beginInfo);
                isRecording = true;
            }

            void Reset()
            {
                vkResetCommandBuffer(handle,0);
            }

            void End()
            {
                vkEndCommandBuffer(handle);
                isRecording = false;
            }

#pragma endregion

#pragma region Draw

            void Draw(uint32_t vertexCount,uint32_t instanceCount,uint32_t firstVertex,uint32_t firstInstance)
            {
                vkCmdDraw(handle,vertexCount,instanceCount,firstVertex,firstInstance);
            }

            void DrawIndirect(VkBuffer buffer,VkDeviceSize offset,uint32_t drawCount,uint32_t stride)
            {
                vkCmdDrawIndirect(handle,buffer,offset,drawCount,stride);
            }

            void DrawIndexed(uint32_t indexCount,uint32_t instanceCount,uint32_t firstIndex,int32_t vertexOffset,uint32_t firstInstance)
            {
                vkCmdDrawIndexed(handle,indexCount,instanceCount,firstIndex,vertexOffset,firstInstance);
            }

            void DrawIndexedIndirectCount(VkBuffer buffer,VkDeviceSize offset,VkBuffer countBuffer,VkDeviceSize countBufferOffset,uint32_t maxDrawCount,uint32_t stride)
            {
                vkCmdDrawIndexedIndirectCount(handle,buffer,offset,countBuffer,countBufferOffset,maxDrawCount,stride);
            }

            void DrawIndexedIndirect(VkBuffer buffer,VkDeviceSize offset,uint32_t drawCount,uint32_t stride)
            {
                vkCmdDrawIndexedIndirect(handle,buffer,offset,drawCount,stride);
            }



#pragma endregion

#pragma region Compute Commands


            void ComputeDispatch(uint32_t x, uint32_t y, uint32_t z)
            {
                vkCmdDispatch(handle,x,y,z);
            }

            void ComputeDispatchBase(uint32_t bx, uint32_t by ,uint32_t bz ,uint32_t x, uint32_t y, uint32_t z)
            {
                vkCmdDispatchBase(handle,bx,by,bz,x,y,z);
            }

            void ComputeDipatchIndirect(VkBuffer buffer, VkDeviceSize offset)
            {
                vkCmdDispatchIndirect(handle,buffer,offset);
            }



#pragma endregion

#pragma region Bind Commands


            void BindPipeline(VkPipelineBindPoint pipelineBindPoint,VkPipeline pipeline)
            {
                vkCmdBindPipeline(handle,pipelineBindPoint,pipeline);
            }

            void BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint,VkPipelineLayout pipelineLayout,uint32_t firstSet,std::vector<VkDescriptorSet>& sets,uint32_t dynamicOffsetCount = 0)
            {
                vkCmdBindDescriptorSets(handle,pipelineBindPoint,pipelineLayout,firstSet,sets.size(),sets.data(),0,nullptr);
            }

            void BindVertexBuffer(uint32_t firstBinding, uint32_t bindingCount, VkBuffer buffer, VkDeviceSize offset)
            {
                vkCmdBindVertexBuffers(handle,firstBinding,bindingCount,&buffer,&offset);
            }

            void BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
            {
                vkCmdBindIndexBuffer(handle,buffer,offset,indexType);
            }



#pragma endregion

#pragma region Utility Commands

            void PushConstants(VkPipelineLayout layout, VkPipelineStageFlags stageFlags,uint32_t offset, uint32_t size, const void* data)
            {
                vkCmdPushConstants(handle,layout,stageFlags,offset,size,data);
            }

            void SetViewport(float x, float y, float width, float height,float minDepth = 0.0f, float maxDepth = 1.0f)
            {
                VkViewport viewport{x,y,width,height,minDepth,maxDepth};
                vkCmdSetViewport(handle,0,1,&viewport);
            }

            void SetViewport(VkViewport& viewport)
            {
                vkCmdSetViewport(handle,0,1,&viewport);
            }

            void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
            {
                VkRect2D scissor {x,y,width,height};
                vkCmdSetScissor(handle,0,1,&scissor);
            }

            void SetScissor(VkRect2D& scissor)
            {
                vkCmdSetScissor(handle,0,1,&scissor);
            }

            void SetViewportScissor(VkViewport& viewport, VkRect2D scissor)
            {
                vkCmdSetViewport(handle,0,1,&viewport);
                vkCmdSetScissor(handle,0,1,&scissor);

            }



#pragma endregion

#pragma region Renderpass

            void BeginRenderpass(VkRenderPass& renderpass,VkFramebuffer& framebuffer,VkRect2D& renderArea,std::vector<VkClearValue>& clearValues)
            {
                VkRenderPassBeginInfo renderpassInfo{};

                renderpassInfo.sType             =   VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderpassInfo.renderPass        =   renderpass;
                renderpassInfo.framebuffer       =   framebuffer;
                renderpassInfo.renderArea        =   renderArea;
                renderpassInfo.clearValueCount   =   clearValues.size();
                renderpassInfo.pClearValues      =   clearValues.data();

                vkCmdBeginRenderPass(handle,&renderpassInfo,VK_SUBPASS_CONTENTS_INLINE);
            }

            void EndRenderpass()
            {
                vkCmdEndRenderPass(handle);
            }

            void NextSubpass()
            {
                vkCmdNextSubpass(handle,VK_SUBPASS_CONTENTS_INLINE);
            }

#pragma endregion

#pragma region Barriers

            void bufferBarrier(VkBuffer buffer, VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage)
            {
                VkBufferMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.srcAccessMask = srcAccess;
                barrier.dstAccessMask = dstAccess;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.buffer = buffer;
                barrier.offset = 0;
                barrier.size = VK_WHOLE_SIZE;
                vkCmdPipelineBarrier(handle, srcStage, dstStage, 0, 0, nullptr, 1, &barrier, 0, nullptr);
            }

            void imageBarrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,VkAccessFlags srcAccess, VkAccessFlags dstAccess,VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT)
            {
                VkImageMemoryBarrier barrier{};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.srcAccessMask = srcAccess;
                barrier.dstAccessMask = dstAccess;
                barrier.oldLayout = oldLayout;
                barrier.newLayout = newLayout;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = image;
                barrier.subresourceRange.aspectMask = aspectMask;
                barrier.subresourceRange.baseMipLevel = 0;
                barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
                vkCmdPipelineBarrier(handle, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
            }


#pragma endregion


#pragma region Copy

            void fillBuffer(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, uint32_t data)
            {
                vkCmdFillBuffer(handle, buffer, offset, size, data);
            }

            void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0)
            {
                VkBufferCopy region{srcOffset, dstOffset, size};
                vkCmdCopyBuffer(handle, src, dst, 1, &region);
            }


#pragma endregion

};
