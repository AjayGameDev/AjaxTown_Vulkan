#include "Renderpass.h"
#include "ImageManager.h"

Renderpass::Renderpass(Context& context,RendererType rendererType,VkSampleCountFlagBits msaaSamples): context(context),msaaSamples(msaaSamples)
{
    CreateRenderPass(rendererType);
}

Renderpass::~Renderpass()
{
    vkDestroyRenderPass(context.device,renderpass,nullptr);
}

void Renderpass::CreateRenderPass(RendererType rendererType)
{
    subpasses.clear();
    attachments.clear();
    dependencies.clear();

    geometryColorAttachmentReferences.clear();
    geometryDepthAttachmentReferences.clear();
    lightingInputAttachmentReferences.clear();
    lightingColorAttachmentReferences.clear();
    lightingDepthAttachmentReferences.clear();
    forwardColorAttachmentReferences.clear();
    forwardDepthAttachmentReferences.clear();
    forwardResolveAttachmentReferences.clear();
    postProcessingInputAttachmentReferences.clear();
    postProcessingColorAttachmentReferences.clear();

    if (rendererType==RendererType::Deferred)
    {
        CreateDeferredAttachments();
        CreateDeferredGeometrySubpass();
        CreateDeferredLightingSubpass();
        CreateDeferredPostProcessingSubpass();
    }
    else if (rendererType == RendererType::Forward)
    {
        CreateForwardAttachments();
        CreateLightingForwardSubpass();
        CreateForwardPostprocessingSubpass();
    }

    VkRenderPassCreateInfo renderpassCreateInfo{};

    renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderpassCreateInfo.attachmentCount  =  static_cast<uint32_t>(attachments.size());
    renderpassCreateInfo.pAttachments     =  attachments.data();
    renderpassCreateInfo.dependencyCount  =  static_cast<uint32_t>(dependencies.size());
    renderpassCreateInfo.pDependencies    =  dependencies.data();
    renderpassCreateInfo.subpassCount     =  static_cast<uint32_t>(subpasses.size());
    renderpassCreateInfo.pSubpasses       =  subpasses.data();

    if (vkCreateRenderPass(context.device,&renderpassCreateInfo,nullptr,&renderpass) != VK_SUCCESS)
        throw std::runtime_error("Can't create renderpass!");
}

void Renderpass::CreateForwardAttachments()
{
    VkAttachmentDescription colorAttachment{};

    colorAttachment.format          =  hdrFormat;
    colorAttachment.samples         =  msaaSamples;
    colorAttachment.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE; // Don't store it back to system memory, instead keep it in tile memory and pass it to the next subpass via input attachment
    colorAttachment.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout     =  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachments.push_back(colorAttachment);

    VkAttachmentDescription depthDescription{};
    depthDescription.format          =  depthFormat;
    depthDescription.samples         =  msaaSamples;
    depthDescription.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthDescription.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthDescription.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthDescription.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthDescription.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    depthDescription.finalLayout     =  VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    attachments.push_back(depthDescription);

    VkAttachmentDescription resolvedColorAttachment{};

    resolvedColorAttachment.format          =  hdrFormat;
    resolvedColorAttachment.samples         =  VK_SAMPLE_COUNT_1_BIT;
    resolvedColorAttachment.loadOp          =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolvedColorAttachment.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE; // Don't store it back to system memory, instead keep it in tile memory and pass it to the next subpass via input attachment
    resolvedColorAttachment.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolvedColorAttachment.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    resolvedColorAttachment.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    resolvedColorAttachment.finalLayout     =  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachments.push_back(resolvedColorAttachment);

    VkAttachmentDescription finalOutput{};
    finalOutput.format          =  context.format.format;
    finalOutput.samples         =  VK_SAMPLE_COUNT_1_BIT;
    finalOutput.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    finalOutput.storeOp         =  VK_ATTACHMENT_STORE_OP_STORE; // store to display on screen through swapchain
    finalOutput.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    finalOutput.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    finalOutput.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    finalOutput.finalLayout     =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Write directly to swapchain instead of writing it to texture and then to swapchain which requires addition subpass or renderpass

    attachments.push_back(finalOutput);

}

void Renderpass::CreateDeferredAttachments()
{
    VkAttachmentDescription albedoDescription{};
    albedoDescription.format          =  diffuseFormat;
    albedoDescription.samples         =  VK_SAMPLE_COUNT_1_BIT;
    albedoDescription.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoDescription.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE; // Don't store it back to system memory, instead keep it in tile memory and pass it to the next subpass via input attachment
    albedoDescription.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    albedoDescription.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    albedoDescription.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    albedoDescription.finalLayout     =  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachments.push_back(albedoDescription);

    VkAttachmentDescription normalDescription{};
    normalDescription.format          =  normalFormat;
    normalDescription.samples         =  VK_SAMPLE_COUNT_1_BIT;
    normalDescription.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalDescription.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    normalDescription.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    normalDescription.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    normalDescription.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    normalDescription.finalLayout     =  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachments.push_back(normalDescription);


    VkAttachmentDescription RMAOMaskDescription{};
    RMAOMaskDescription.format          =  RMAOFormat;
    RMAOMaskDescription.samples         =  VK_SAMPLE_COUNT_1_BIT;
    RMAOMaskDescription.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    RMAOMaskDescription.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    RMAOMaskDescription.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    RMAOMaskDescription.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    RMAOMaskDescription.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    RMAOMaskDescription.finalLayout     =  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachments.push_back(RMAOMaskDescription);


    VkAttachmentDescription depthDescription{};
    depthDescription.format          =  depthFormat;
    depthDescription.samples         =  VK_SAMPLE_COUNT_1_BIT;
    depthDescription.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthDescription.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthDescription.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthDescription.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthDescription.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    depthDescription.finalLayout     =  VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    attachments.push_back(depthDescription);

    VkAttachmentDescription lightingpassHDROutput{};
    lightingpassHDROutput.format          =  hdrFormat;
    lightingpassHDROutput.samples         =  VK_SAMPLE_COUNT_1_BIT;
    lightingpassHDROutput.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    lightingpassHDROutput.storeOp         =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    lightingpassHDROutput.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    lightingpassHDROutput.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    lightingpassHDROutput.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    lightingpassHDROutput.finalLayout     =  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachments.push_back(lightingpassHDROutput);

    // Final output [Index 5]
    VkAttachmentDescription finalOutput{};
    finalOutput.format          =  context.format.format;
    finalOutput.samples         =  VK_SAMPLE_COUNT_1_BIT;
    finalOutput.loadOp          =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    finalOutput.storeOp         =  VK_ATTACHMENT_STORE_OP_STORE; // store to display on screen through swapchain
    finalOutput.stencilLoadOp   =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    finalOutput.stencilStoreOp  =  VK_ATTACHMENT_STORE_OP_DONT_CARE;
    finalOutput.initialLayout   =  VK_IMAGE_LAYOUT_UNDEFINED;
    finalOutput.finalLayout     =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Write directly to swapchain instead of writing it to texture and then to swapchain which requires addition subpass or renderpass

    attachments.push_back(finalOutput);
}

void Renderpass::CreateLightingForwardSubpass()
{
    // References for all color attachments and depth attachments
    forwardColorAttachmentReferences   =     {    {   0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL         }   };

    forwardDepthAttachmentReferences   =     {    {   1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }   };

    forwardResolveAttachmentReferences =     {    {   2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }   };


    // Subpass dependency External-->Subpass 0
    VkSubpassDependency forwardpassDependency{};
    forwardpassDependency.srcSubpass       =   VK_SUBPASS_EXTERNAL; // External
    forwardpassDependency.dstSubpass       =   0; // Geometry pass
    forwardpassDependency.srcStageMask     =   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // wait for any previous work to complete (like compute culling)
    forwardpassDependency.dstStageMask     =   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; // before writing to color buffer or depth buffer
    forwardpassDependency.srcAccessMask    =   VK_ACCESS_MEMORY_READ_BIT; // Complete previous memory reads
    forwardpassDependency.dstAccessMask    =   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // before writing to color buffer or depth buffer
    forwardpassDependency.dependencyFlags  =   VK_DEPENDENCY_BY_REGION_BIT; // Essential for performance gain on tile based gpus

    dependencies.push_back(forwardpassDependency);

    // forward subpass
    VkSubpassDescription forwardpassDescription{};
    forwardpassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    forwardpassDescription.colorAttachmentCount    = forwardColorAttachmentReferences.size();
    forwardpassDescription.pColorAttachments       = forwardColorAttachmentReferences.data();
    forwardpassDescription.pDepthStencilAttachment = forwardDepthAttachmentReferences.data();
    forwardpassDescription.pResolveAttachments     = forwardResolveAttachmentReferences.data();


    subpasses.push_back(forwardpassDescription);
}

void Renderpass::CreateForwardPostprocessingSubpass()
{
    // References for all color attachments and depth attachments
    postProcessingInputAttachmentReferences   =     {    {   2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL         }   };  // resolved hdr image
    postProcessingColorAttachmentReferences   =     {    {   3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL         }   }; //  final swapchain image


    // Subpass dependency subpass 0 --> Subpass 1
    VkSubpassDependency postprocessingDependency{};
    postprocessingDependency.srcSubpass       =   0; // forward subpass
    postprocessingDependency.dstSubpass       =   1; // Post processing subpass
    postprocessingDependency.srcStageMask     =   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    postprocessingDependency.dstStageMask     =   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    postprocessingDependency.srcAccessMask    =   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    postprocessingDependency.dstAccessMask    =   VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    postprocessingDependency.dependencyFlags  =   VK_DEPENDENCY_BY_REGION_BIT; // Essential for performance gain on tile based gpus

    dependencies.push_back(postprocessingDependency);

    // Subpass dependency subpass 1 --> External
    VkSubpassDependency presentationDependency{};
    presentationDependency.srcSubpass       =   1; // Post processing subpass
    presentationDependency.dstSubpass       =   VK_SUBPASS_EXTERNAL; // External
    presentationDependency.srcStageMask     =   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    presentationDependency.dstStageMask     =   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    presentationDependency.srcAccessMask    =   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    presentationDependency.dstAccessMask    =   VK_ACCESS_MEMORY_READ_BIT;
    presentationDependency.dependencyFlags  =   VK_DEPENDENCY_BY_REGION_BIT; // Essential for performance gain on tile based gpus

    dependencies.push_back(presentationDependency);

    // post-processing subpass
    VkSubpassDescription postprocessingDescription{};
    postprocessingDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    postprocessingDescription.colorAttachmentCount    = postProcessingColorAttachmentReferences.size();
    postprocessingDescription.pColorAttachments       = postProcessingColorAttachmentReferences.data();
    postprocessingDescription.inputAttachmentCount    = postProcessingInputAttachmentReferences.size();
    postprocessingDescription.pInputAttachments       = postProcessingInputAttachmentReferences.data();


    subpasses.push_back(postprocessingDescription);
}


void Renderpass::CreateDeferredGeometrySubpass()
{

    // References for all color attachments and depth attachments
    geometryColorAttachmentReferences =
    {
        {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},   // albedo
        {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  //  normal
        {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}, //   RMAO mask
    };

    geometryDepthAttachmentReferences = {{3, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};




    // Subpass dependency External-->Subpass 0
    VkSubpassDependency geometrypassDependency{};
    geometrypassDependency.srcSubpass       =   VK_SUBPASS_EXTERNAL; // External
    geometrypassDependency.dstSubpass       =   0; // Geometry pass
    geometrypassDependency.srcStageMask     =   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // wait for any previous work to complete (like compute culling)
    geometrypassDependency.dstStageMask     =   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; // before writing to color buffer or depth buffer
    geometrypassDependency.srcAccessMask    =   VK_ACCESS_MEMORY_READ_BIT; // Complete previous memory reads
    geometrypassDependency.dstAccessMask    =   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // before writing to color buffer or depth buffer
    geometrypassDependency.dependencyFlags  =   VK_DEPENDENCY_BY_REGION_BIT; // Essential for performance gain on tile based gpus

    dependencies.push_back(geometrypassDependency);

    // Geometry subpass
    VkSubpassDescription geometrypassDescription{};
    geometrypassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    geometrypassDescription.colorAttachmentCount = geometryColorAttachmentReferences.size();
    geometrypassDescription.pColorAttachments = geometryColorAttachmentReferences.data();
    geometrypassDescription.pDepthStencilAttachment = geometryDepthAttachmentReferences.data();

    subpasses.push_back(geometrypassDescription);

}

void Renderpass::CreateDeferredLightingSubpass()
{
    lightingInputAttachmentReferences =
    {
      {0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
      {1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
      {2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
    };

    lightingColorAttachmentReferences = { { 4, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
    lightingDepthAttachmentReferences = { { 3,VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL } };

    // Subpass dependency Subpass0-->Subpass1
    VkSubpassDependency lightingpassDependency{};

    lightingpassDependency.srcSubpass       =  0;  // Geometry pass
    lightingpassDependency.dstSubpass       =  1; //  Lighting pass
    lightingpassDependency.srcStageMask     =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait for gbuffer to complete writing
    lightingpassDependency.dstStageMask     =  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; // before starting fragment shader
    lightingpassDependency.srcAccessMask    =  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Writing to color attachments should be completed first
    lightingpassDependency.dstAccessMask    =  VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; // before we start reading from them as input attachments
    lightingpassDependency.dependencyFlags  =  VK_DEPENDENCY_BY_REGION_BIT;

    dependencies.push_back(lightingpassDependency);

    // Lighting subpass
    VkSubpassDescription lightingpasssDescription{};

    lightingpasssDescription.pipelineBindPoint        =  VK_PIPELINE_BIND_POINT_GRAPHICS;
    lightingpasssDescription.inputAttachmentCount     =  lightingInputAttachmentReferences.size();
    lightingpasssDescription.pInputAttachments        =  lightingInputAttachmentReferences.data();
    lightingpasssDescription.colorAttachmentCount     =  lightingColorAttachmentReferences.size();
    lightingpasssDescription.pColorAttachments        =  lightingColorAttachmentReferences.data();
    lightingpasssDescription.pDepthStencilAttachment  =  lightingDepthAttachmentReferences.data();

    subpasses.push_back(lightingpasssDescription);

}

void Renderpass::CreateDeferredPostProcessingSubpass()
{
    postProcessingInputAttachmentReferences = { { 4, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } }; // HDR image

    postProcessingColorAttachmentReferences = { { 5,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }; // Final Output


    // Subpass dependency Subpass1-->Subpass2
    VkSubpassDependency postprocessingpasssDependency{};
    postprocessingpasssDependency.srcSubpass       =  1;  // Lighting pass
    postprocessingpasssDependency.dstSubpass       =  2; //  Post processing
    postprocessingpasssDependency.srcStageMask     =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    postprocessingpasssDependency.dstStageMask     =  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    postprocessingpasssDependency.srcAccessMask    =  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    postprocessingpasssDependency.dstAccessMask    =  VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    postprocessingpasssDependency.dependencyFlags  =  VK_DEPENDENCY_BY_REGION_BIT;

    dependencies.push_back(postprocessingpasssDependency);

    // Subpass dependency Subpass2--> External[Swapchain]
    VkSubpassDependency presentationDependency{};
    presentationDependency.srcSubpass       =  2;  // Post processing
    presentationDependency.dstSubpass       =  VK_SUBPASS_EXTERNAL; //  External
    presentationDependency.srcStageMask     =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    presentationDependency.dstStageMask     =  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    presentationDependency.srcAccessMask    =  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    presentationDependency.dstAccessMask    =  VK_ACCESS_MEMORY_READ_BIT;
    presentationDependency.dependencyFlags  =  VK_DEPENDENCY_BY_REGION_BIT;

    dependencies.push_back(presentationDependency);

    // Post-processing subpass
    VkSubpassDescription postprocessingpasssDescription{};
    postprocessingpasssDescription.pipelineBindPoint     =  VK_PIPELINE_BIND_POINT_GRAPHICS;
    postprocessingpasssDescription.inputAttachmentCount  =  postProcessingInputAttachmentReferences.size();
    postprocessingpasssDescription.pInputAttachments     =  postProcessingInputAttachmentReferences.data();
    postprocessingpasssDescription.colorAttachmentCount  =  postProcessingColorAttachmentReferences.size();
    postprocessingpasssDescription.pColorAttachments     =  postProcessingColorAttachmentReferences.data();


    subpasses.push_back(postprocessingpasssDescription);

}
