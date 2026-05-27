#pragma once

#include "Fence.h"
#include "Semaphore.h"
#include "Swapchain.h"
#include "CommandPool.h"
#include "CommandBuffer.h"

struct FrameResource
{
    Fence         inFlightFence;
    Semaphore     imageAvailableSemaphore;
    Semaphore     renderingFinishedSemaphore;
    Semaphore     computeFinishedSemaphore;
    Semaphore     transferFinsihedSemaphore;

    CommandPool   graphicsCommandPool;
    CommandPool   computeCommandPool;
    CommandPool   tranferCommandPool;

    CommandBuffer graphicsCommandBuffer;
    CommandBuffer computeCommandBuffer;
    CommandBuffer transferCommandBuffer;


};


class Renderer
{
    private:
            Context& context;
            Swapchain& swapchain;
            uint32_t currentFrame = 0;
            uint32_t maxFramesInFlight = 0;
            uint32_t currentImageIndex = 0;
            bool shouldCloseWindow = false;
            std::vector<FrameResource> frameResources;
            VkViewport viewport{};
            VkRect2D   scissor{};
            RendererType rendererType;
            VkSampleCountFlagBits samples;
    public:
            Renderer(Context& context,Swapchain& swapchain,const uint8_t maxFramesInFlight,RendererType rendererType,VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT) : context(context),swapchain(swapchain),maxFramesInFlight(maxFramesInFlight),rendererType(rendererType),samples(samples)
            {
                frameResources.reserve(maxFramesInFlight);

                for (int i = 0; i < maxFramesInFlight; ++i)
                {
                    FrameResource frameResource
                    {
                        Fence(context),
                        Semaphore(context),
                        Semaphore(context),
                        Semaphore(context),
                        Semaphore(context),
                        CommandPool(context),
                        CommandPool(context),
                        CommandPool(context)
                    };

                    frameResources.push_back(std::move(frameResource));

                }

                for (int i = 0; i < maxFramesInFlight; ++i)
                {
                    frameResources[i].graphicsCommandPool.Create(context.graphicsFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
                    frameResources[i].computeCommandPool.Create(context.computeFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
                    frameResources[i].tranferCommandPool.Create(context.transferFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

                    // Allocate command buffers
                    frameResources[i].graphicsCommandBuffer = frameResources[i].graphicsCommandPool.Allocate();
                    frameResources[i].computeCommandBuffer  = frameResources[i].computeCommandPool.Allocate();
                    frameResources[i].transferCommandBuffer = frameResources[i].tranferCommandPool.Allocate();
                }

                // Set dynamic viewport

                viewport.x = 0;
                viewport.y = 0;
                viewport.width  = static_cast<float>(swapchain.swapChainCreateInfo.imageExtent.width);
                viewport.height = static_cast<float>(swapchain.swapChainCreateInfo.imageExtent.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;


                // Set dynamic Scissor

                scissor.offset    =   {0,0};
                scissor.extent    =   swapchain.swapChainCreateInfo.imageExtent;
            }

            RendererType GetRendererType()
            {
                return rendererType;
            }
            VkSampleCountFlagBits GetSamplesCount()
            {
                return samples;
            }
            bool ShouldCloseWindow()
            {
                SDL_Event e;
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_EVENT_QUIT)
                        shouldCloseWindow = true;
                }
                return shouldCloseWindow;
            }
            uint8_t GetCurrentFrameIndex()
            {
                return currentFrame;
            }
            uint32_t GetCurrentImageIndex()
            {
                return currentImageIndex;
            }
            void AcquireImage()
            {
                frameResources[currentFrame].inFlightFence.WaitAndReset();
                // Only call wait then reset when fence is created with signaled flag other wise it will freeze the prgram on first frame [Order matters here]
                vkAcquireNextImageKHR(context.device,swapchain.GetHandle(),UINT64_MAX,frameResources[currentFrame].imageAvailableSemaphore.GetHandle(),nullptr,&currentImageIndex);
                // Semaphore, fence or both can be used here for sync but since we are using semaphore, fence can be nullptr
            }

            void SubmitGraphicsQueue()
            {
                VkSemaphore     waitSemaphore[]                =
                    {
                        frameResources[currentFrame].imageAvailableSemaphore.GetHandle(), // Need to create this
                        frameResources[currentFrame].computeFinishedSemaphore.GetHandle()
                    };

                VkPipelineStageFlags waitStages[] =
                    {
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // order matters based on wait semaphores
                        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
                    };

                VkSemaphore&     renderingFinishedSemaphore   =   frameResources[currentFrame].renderingFinishedSemaphore.GetHandle();
                VkCommandBuffer& graphicsCommandBuffer        =   frameResources[currentFrame].graphicsCommandBuffer.GetHandle();


                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount    =  1;
                submitInfo.pCommandBuffers       =  &graphicsCommandBuffer;

                // stop signals for the gpu [imageAvailableSemaphore will be signaled by vkAcquireNextImageKHR, it ensures gpu doesn't draw untill valid swapchain image in avaialble]
                submitInfo.waitSemaphoreCount    =  2;
                submitInfo.pWaitSemaphores       =  waitSemaphore;

                // Instead of stopping entire gpu, it tells gpu the exact stage to wait at. Order of wait stages correspond to the order of wait semaphore array [VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT with imageAvaialable semaphore and VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT with compute finished semaphore]
                submitInfo.pWaitDstStageMask     =  waitStages;

                // Signal the presentation engine that it can safely present the swapchain image on the screen
                submitInfo.signalSemaphoreCount  =  1;
                submitInfo.pSignalSemaphores     =  &renderingFinishedSemaphore;

                // Fence gives green signal to the cpu that it safe to use resources now as gpu is done
                vkQueueSubmit(context.graphicsQueue,1,&submitInfo,frameResources[currentFrame].inFlightFence.GetHandle());
            }

            void SubmitComputeQueue()
            {

                VkCommandBuffer& computeCommandBuffer = frameResources[currentFrame].computeCommandBuffer.GetHandle();
                VkSemaphore&     computeFinishedSemaphore = frameResources[currentFrame].computeFinishedSemaphore.GetHandle();

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &computeCommandBuffer;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &computeFinishedSemaphore;

                vkQueueSubmit(context.computeQueue,1,&submitInfo,VK_NULL_HANDLE);
            }


            void PresentImage()
            {
                VkSwapchainKHR& swapchainHandle              =  swapchain.GetHandle();
                VkSemaphore&    renderingFinsihedSemaphore   =  frameResources[currentFrame].renderingFinishedSemaphore.GetHandle();

                VkPresentInfoKHR presentInfo{};
                presentInfo.sType                =   VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                presentInfo.swapchainCount       =   1;
                presentInfo.pSwapchains          =   &swapchainHandle;
                presentInfo.pImageIndices        =   &currentImageIndex;
                presentInfo.waitSemaphoreCount   =   1;
                presentInfo.pWaitSemaphores      =   &renderingFinsihedSemaphore;

                vkQueuePresentKHR(context.graphicsQueue,&presentInfo);
            }

            void AdvanceFrame()
            {
                currentFrame = (currentFrame + 1) % maxFramesInFlight; // inline by default as it is declared inside the header class
            }

            void ResetGraphicsCommandBuffer()
            {
                frameResources[currentFrame].graphicsCommandBuffer.Reset();
            }

            void ResetComputeCommandBuffer()
            {
                frameResources[currentFrame].computeCommandBuffer.Reset();
            }

            void BeginGraphicsCommandBuffer()
            {
                frameResources[currentFrame].graphicsCommandBuffer.Begin();
            }

            void BeginComputeCommandBuffer()
            {
                frameResources[currentFrame].computeCommandBuffer.Begin();
            }

            void EndGraphicsCommandBuffer()
            {
                frameResources[currentFrame].graphicsCommandBuffer.End();
            }

            void EndComputeCommandBuffer()
            {
                frameResources[currentFrame].computeCommandBuffer.End();
            }

            void SetViewportScissor()
            {
                frameResources[currentFrame].graphicsCommandBuffer.SetViewportScissor(viewport,scissor);
            }

            void BeginRenderpass(VkRenderPass& renderpass,VkFramebuffer& framebuffer)
            {
                // TO do

                VkRect2D renderArea;
                renderArea.offset = {0,0};
                renderArea.extent = swapchain.swapChainCreateInfo.imageExtent;

                std::vector<VkClearValue> clearValues;

                if (rendererType==RendererType::Deferred)
                {
                    clearValues.resize(6);

                    // Attachments 0-2: G-buffer color attachments
                    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};       // Albedo
                    clearValues[1].color = {{0.0f, 0.0f, 0.0f, 1.0f}};      // Normal
                    clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};     // RMAO
                    //clearValues[3].depthStencil = {1.0f, 0};            // Attachment 3: DEPTH - MUST use depthStencil, not color! [for standard z]
                    clearValues[3].depthStencil = {0.0f, 0};             // Attachment 3: for reverse z depth
                    clearValues[4].color = {{0.0f, 0.0f, 0.0f, 1.0f}};  // Attachment 4: HDR output
                    clearValues[5].color = {{0.1f, 0.2f, 0.3f, 1.0f}}; // Attachment 5: Final output (what you see on screen)
                }
                else if (rendererType==RendererType::Forward)
                {
                    clearValues.resize(4);

                    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};        // msaa hdr color
                    //clearValues[1].depthStencil = {1.0f, 0};               //  msaa DEPTH - MUST use depthStencil, not color! [for standard z]
                    clearValues[1].depthStencil = {0.0f, 0};                //   for reverse z depth
                    clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};     //   resolved hdr
                    clearValues[3].color = {{0.1f, 0.2f, 0.3f, 1.0f}};    //    final output
                }


                frameResources[currentFrame].graphicsCommandBuffer.BeginRenderpass(renderpass,framebuffer,renderArea,clearValues);
            }

            void EndRenderpass()
            {
                frameResources[currentFrame].graphicsCommandBuffer.EndRenderpass();
            }

            void BindGraphicsPipeline(VkPipeline& pipeline)
            {
                frameResources[currentFrame].graphicsCommandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS,pipeline);
            }

            void BindComputePipeline(VkPipeline& pipeline)
            {
                frameResources[currentFrame].computeCommandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE,pipeline);
            }

            void PushConstant_compute(VkPipelineLayout& pipelineLayout,VkShaderStageFlags shaderFlags,uint32_t offset,uint32_t size,const void* pushConstantData)
            {
                vkCmdPushConstants(frameResources[currentFrame].computeCommandBuffer.GetHandle(),pipelineLayout,shaderFlags,offset,size,pushConstantData);
            }

            void PushConstant_graphics(VkPipelineLayout& pipelineLayout,VkShaderStageFlags shaderFlags,uint32_t offset,uint32_t size,const void* pushConstantData)
            {
                vkCmdPushConstants(frameResources[currentFrame].graphicsCommandBuffer.GetHandle(),pipelineLayout,shaderFlags,offset,size,pushConstantData);
            }

            void BindVertexBuffer(uint32_t firstBinding,uint32_t bindingCount,VkBuffer& buffer,VkDeviceSize& offset)
            {
                frameResources[currentFrame].graphicsCommandBuffer.BindVertexBuffer(firstBinding,bindingCount,buffer,offset);
            }

            void BindIndexBuffer(VkBuffer& buffer,VkDeviceSize& offset,VkIndexType indexType)
            {
                frameResources[currentFrame].graphicsCommandBuffer.BindIndexBuffer(buffer,offset,indexType);
            }

            void BindDescriptorSetGraphics(VkPipelineLayout& pipelineLayout,VkDescriptorSet& descriptorSet)
            {
                vkCmdBindDescriptorSets(frameResources[currentFrame].graphicsCommandBuffer.GetHandle(),VK_PIPELINE_BIND_POINT_GRAPHICS,pipelineLayout,0,1,&descriptorSet,0,NULL);
            }

            void BindDescriptorSetCompute(VkPipelineLayout& pipelineLayout,VkDescriptorSet& descriptorSet)
            {
                vkCmdBindDescriptorSets(frameResources[currentFrame].computeCommandBuffer.GetHandle(),VK_PIPELINE_BIND_POINT_COMPUTE,pipelineLayout,0,1,&descriptorSet,0,NULL);
            }

            void DispatchComputeShader(uint32_t x,uint32_t y, uint32_t z)
            {
                vkCmdDispatch(frameResources[currentFrame].computeCommandBuffer.GetHandle(),x,y,z);
            }

            void Draw(uint32_t vertexCount,uint32_t instanceCount,uint32_t firstVertex,uint32_t firstInstance)
            {
                frameResources[currentFrame].graphicsCommandBuffer.Draw(vertexCount,instanceCount,firstVertex,firstInstance);
            }

            void DrawIndirect(VkBuffer indirectBuffer,VkDeviceSize offset,uint32_t drawCount,uint32_t stride)
            {
                frameResources[currentFrame].graphicsCommandBuffer.DrawIndirect(indirectBuffer,offset,drawCount,stride);
            }

            void DrawIndexedIndirect(VkBuffer indirectBuffer,VkDeviceSize offset,uint32_t drawCount,uint32_t stride)
            {
                frameResources[currentFrame].graphicsCommandBuffer.DrawIndexedIndirect(indirectBuffer,offset,drawCount,stride);
            }

            void DrawIndexedIndirectCount(VkBuffer indirectBuffer,VkDeviceSize offset,VkBuffer countBuffer,VkDeviceSize countOffset,uint32_t maxDrawCount,uint32_t stride)
            {
                frameResources[currentFrame].graphicsCommandBuffer.DrawIndexedIndirectCount(indirectBuffer,offset,countBuffer,countOffset,maxDrawCount,stride);
            }

            void NextSubpass()
            {
                frameResources[currentFrame].graphicsCommandBuffer.NextSubpass();
            }

            void ResetDrawCountBuffer(VkBuffer& drawCountBuffer)
            {
                vkCmdFillBuffer(frameResources[currentFrame].computeCommandBuffer.GetHandle(),drawCountBuffer,0,sizeof(uint32_t),0);
            }

            void ResetDrawCountBarrier(VkBuffer& buffer_drawCount)
            {
                VkBufferMemoryBarrier barrier{};

                barrier.sType                =  VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barrier.srcAccessMask        =  VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask        =  VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                barrier.srcQueueFamilyIndex  =  VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex  =  VK_QUEUE_FAMILY_IGNORED;
                barrier.buffer               =  buffer_drawCount;
                barrier.offset               =  0;
                barrier.size                 =  VK_WHOLE_SIZE;

                vkCmdPipelineBarrier
                (
                                     frameResources[currentFrame].computeCommandBuffer.GetHandle(),
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,        //     source stage mask
                                     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, //      destination stage mask
                                     0, // dependency flags
                                     0,nullptr, // memory barrier count, memory barrier
                                     1,&barrier, // buffer memory cout, buffer barrier
                                     0,nullptr // Image barrier count, Image barrier
                );


            }
            void ComputeToIndirectBarrier(VkBuffer& buffer_drawCommands,VkBuffer& buffer_drawCount)
            {
                VkBufferMemoryBarrier barriers[2]{};

                barriers[0].sType                =  VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                barriers[0].srcAccessMask        =  VK_ACCESS_SHADER_WRITE_BIT;
                barriers[0].dstAccessMask        =  VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
                barriers[0].srcQueueFamilyIndex  =  VK_QUEUE_FAMILY_IGNORED;
                barriers[0].dstQueueFamilyIndex  =  VK_QUEUE_FAMILY_IGNORED;
                barriers[0].buffer               =  buffer_drawCommands;
                barriers[0].offset               =  0;
                barriers[0].size                 =  VK_WHOLE_SIZE;

                barriers[1] = barriers[0];
                barriers[1].buffer = buffer_drawCount;

                vkCmdPipelineBarrier
                (
                                     frameResources[currentFrame].computeCommandBuffer.GetHandle(),
                                     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, //     source stage mask
                                     VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, //      destination stage mask
                                     0, // dependency flags
                                     0,nullptr, // memory barrier count, memory barrier
                                     2,barriers, // buffer memory cout, buffer barrier
                                     0,nullptr // Image barrier count, Image barrier
                );


            }


};