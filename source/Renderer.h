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
            uint8_t currentFrame = 0;
            uint8_t maxFramesInFlight = 0;
            uint32_t currentImageIndex = 0;
            bool shouldCloseWindow = false;
            std::vector<FrameResource> frameResources;
            VkViewport viewport{};
            VkRect2D scissor{};

    public:
            Renderer(Context& context,Swapchain& swapchain,const uint8_t maxFramesInFlight) : context(context),swapchain(swapchain),maxFramesInFlight(maxFramesInFlight)
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
                    frameResources[i].computeCommandBuffer = frameResources[i].computeCommandPool.Allocate();
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
                VkSemaphore&     waitSemaphore                =   frameResources[currentFrame].imageAvailableSemaphore.GetHandle(); // Need to create this
                VkSemaphore&     renderingFinishedSemaphore   =   frameResources[currentFrame].renderingFinishedSemaphore.GetHandle();
                VkCommandBuffer& graphicsCommandBuffer        =   frameResources[currentFrame].graphicsCommandBuffer.GetHandle();
                VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };


                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pWaitDstStageMask = waitStages;
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = &waitSemaphore;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &renderingFinishedSemaphore;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &graphicsCommandBuffer;
                vkQueueSubmit(context.graphicsQueue,1,&submitInfo,frameResources[currentFrame].inFlightFence.GetHandle());
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

            void BeginGraphicsCommandBuffer()
            {
                frameResources[currentFrame].graphicsCommandBuffer.Begin();
            }

            void EndGraphicsCommandBuffer()
            {
                frameResources[currentFrame].graphicsCommandBuffer.End();
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

                std::vector<VkClearValue> clearValues(6);

                // Attachments 0-2: G-buffer color attachments
                clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};       // Albedo
                clearValues[1].color = {{0.0f, 0.0f, 0.0f, 1.0f}};      // Normal
                clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};     // RMAO
                clearValues[3].depthStencil = {1.0f, 0};              // Attachment 3: DEPTH - MUST use depthStencil, not color!
                clearValues[4].color = {{0.0f, 0.0f, 0.0f, 1.0f}};   // Attachment 4: HDR output
                clearValues[5].color = {{0.1f, 0.2f, 0.3f, 1.0f}};  // Attachment 5: Final output (what you see on screen)

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

            void BindVertexBuffer(uint32_t firstBinding,uint32_t bindingCount,VkBuffer& buffer,VkDeviceSize& offset)
            {
                frameResources[currentFrame].graphicsCommandBuffer.BindVertexBuffer(firstBinding,bindingCount,buffer,offset);
            }

            void Draw(uint32_t vertexCount,uint32_t instanceCount,uint32_t firstVertex,uint32_t firstInstance)
            {
                frameResources[currentFrame].graphicsCommandBuffer.Draw(vertexCount,instanceCount,firstVertex,firstInstance);
            }

            void NextSubpass()
            {
                frameResources[currentFrame].graphicsCommandBuffer.NextSubpass();
            }


};
