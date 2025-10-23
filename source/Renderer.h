#pragma once

#include "vector"
#include "Fence.h"
#include "Semaphore.h"
#include "VulkanContext.h"
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
            VulkanContext& context;
            uint8_t currentFrame = 0;
            uint8_t maxFramesInFlight = 0;
            uint32_t imageIndex = 0;
            bool shouldCloseWindow = false;
            std::vector<FrameResource> frameResources;

    public:
            Renderer(VulkanContext& context,const uint8_t maxFramesInFlight) : context(context),maxFramesInFlight(maxFramesInFlight)
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
                        Semaphore(context)
                    };

                    frameResources.push_back(std::move(frameResource));

                }
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

            void AcquireImageFromSwapchain(Swapchain& swapchain)
            {
                frameResources[currentFrame].inFlightFence.WaitAndReset();
                // Only call wait then reset when fence is created with signaled flag other wise it will freeze the prgram on first frame [Order matters here]
                vkAcquireNextImageKHR(context.device,swapchain.GetHandle(),UINT64_MAX,frameResources[currentFrame].imageAvailableSemaphore.GetHandle(),nullptr,&imageIndex);
                // Semaphore, fence or both can be used here for sync but since we are using semaphore, fence can be nullptr
            }

            void SubmitGraphicsQueue()
            {
                VkSemaphore     waitSemaphore                =   frameResources[currentFrame].imageAvailableSemaphore.GetHandle(); // Need to create this
                VkSemaphore     renderingFinishedSemaphore   =   frameResources[currentFrame].renderingFinishedSemaphore.GetHandle();
                VkCommandBuffer graphicsCommandBuffer        =   frameResources[currentFrame].graphicsCommandBuffer.GetHandle();


                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = &waitSemaphore;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &renderingFinishedSemaphore;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &graphicsCommandBuffer;
                vkQueueSubmit(context.graphicsQueue,1,&submitInfo,frameResources[currentFrame].inFlightFence.GetHandle());
            }

            void PresentImage(Swapchain& swapchain)
            {
                VkSwapchainKHR swapchainHandle              =  swapchain.GetHandle();
                VkSemaphore    renderingFinsihedSemaphore   =  frameResources[currentFrame].renderingFinishedSemaphore.GetHandle();

                VkPresentInfoKHR presentInfo{};
                presentInfo.sType                =   VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                presentInfo.swapchainCount       =   1;
                presentInfo.pSwapchains          =   &swapchainHandle;
                presentInfo.pImageIndices        =   &imageIndex;
                presentInfo.waitSemaphoreCount   =   1;
                presentInfo.pWaitSemaphores      =   &renderingFinsihedSemaphore;

                vkQueuePresentKHR(context.graphicsQueue,&presentInfo);
            }

            void AdvanceFrame()
            {
                currentFrame = (currentFrame + 1) % maxFramesInFlight; // inline by default as it is declared inside the header class
            }


};
