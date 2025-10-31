#pragma once

#include "Context.h"

class Renderpass 
{
    private:


            Context& context;

            std::vector<VkAttachmentDescription>  attachments;
            std::vector<VkSubpassDescription>     subpasses;
            std::vector<VkSubpassDependency>      dependencies;

            std::vector<VkAttachmentReference> geometryColorAttachmentReferences;
            std::vector<VkAttachmentReference> geometryDepthAttachmentReferences;

            std::vector<VkAttachmentReference> lightingInputAttachmentReferences;
            std::vector<VkAttachmentReference> lightingColorAttachmentReferences;
            std::vector<VkAttachmentReference> lightingDepthAttachmentReferences;

            std::vector<VkAttachmentReference> postProcessingInputAttachmentReferences;
            std::vector<VkAttachmentReference> postProcessingColorAttachmentReferences;

            void CreateRenderPass();
            void CreateAttachments();
            void CreateGeometrySubpass(); // Tile renderer's benifit greatly by working on tile memory instead of storing output back to ram
                                                            // we can leveage this by creating multiple subpasses instead of separate renderpasses[NOTE: it won't be benificial on desktop]
            void CreateLightingSubpass();
            void CreatePostProcessingSubpass();

    public:
            VkRenderPass  renderpass = nullptr;
            VkRenderPass& GetHandle() { return renderpass; }
            explicit Renderpass(Context& context);
            ~Renderpass();
};
