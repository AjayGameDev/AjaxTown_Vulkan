#pragma once

#include "VulkanContext.h"
#include <vector>

class Renderpass 
{
    private:


            VulkanContext& context;

            std::vector<VkAttachmentDescription>  attachments;
            std::vector<VkSubpassDescription>     subpasses;
            std::vector<VkSubpassDependency>      dependencies;

            void CreateRenderPass();
            void CreateAttachments();
            void CreateGeometrySubpass(); // Tile renderer's benifit greatly by working on tile memory instead of storing output back to ram
                                                            // we can leveage this by creating multiple subpasses instead of separate renderpasses[NOTE: it won't be benificial on desktop]
            void CreateLightingSubpass();
            void CreatePostProcessingSubpass();

    public:
            VkRenderPass renderpass = nullptr;
            explicit Renderpass(VulkanContext& context);
            ~Renderpass();
};
