#pragma once


class Renderpass 
{
    private:


            Context& context;
            VkSampleCountFlagBits msaaSamples;

            std::vector<VkAttachmentDescription>  attachments;
            std::vector<VkSubpassDescription>     subpasses;
            std::vector<VkSubpassDependency>      dependencies;

            std::vector<VkAttachmentReference> geometryColorAttachmentReferences;
            std::vector<VkAttachmentReference> geometryDepthAttachmentReferences;

            std::vector<VkAttachmentReference> lightingInputAttachmentReferences;
            std::vector<VkAttachmentReference> lightingColorAttachmentReferences;
            std::vector<VkAttachmentReference> lightingDepthAttachmentReferences;

            std::vector<VkAttachmentReference> forwardColorAttachmentReferences;
            std::vector<VkAttachmentReference> forwardDepthAttachmentReferences;
            std::vector<VkAttachmentReference> forwardResolveAttachmentReferences;

            std::vector<VkAttachmentReference> postProcessingInputAttachmentReferences;
            std::vector<VkAttachmentReference> postProcessingColorAttachmentReferences;



            void CreateRenderPass(RendererType rendererType);

            void CreateDeferredAttachments();
            void CreateForwardAttachments();

            void CreateDeferredGeometrySubpass();  // Tile renderer's benifit greatly by working on tile memory instead of storing output back to ram
            void CreateDeferredLightingSubpass(); // we can leveage this by creating multiple subpasses instead of separate renderpasses[NOTE: it won't be benificial on desktop]
            void CreateDeferredPostProcessingSubpass();

            void CreateLightingForwardSubpass();
            void CreateForwardPostprocessingSubpass();


    public:
            VkRenderPass  renderpass = nullptr;
            VkRenderPass& GetHandle() { return renderpass; }
            explicit Renderpass(Context& context,RendererType rendererType,VkSampleCountFlagBits msaaSamples);
            ~Renderpass();
};
