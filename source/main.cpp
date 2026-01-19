#include "Headers.h"

int main(int argc,char* argv[])
{
    Window window("Ajax Town",640*3,480*3);
    Context context(window);
    Swapchain swapchain(context);
    Renderer renderer(context,swapchain,swapchain.imageCount,RendererType::Forward,VK_SAMPLE_COUNT_8_BIT);
    Renderpass renderpass(context,renderer.GetRendererType(),renderer.GetSamplesCount());
    ImageManager imageManager(context);
    BufferManager bufferManager(context); 
    Framebuffer framebuffer(context,swapchain,imageManager,renderpass,renderer.GetRendererType(),renderer.GetSamplesCount());
    Shader triangleShader(context,"Triangle");
    Shader postprocessingShader(context,"PostProcessing");

    std::vector<Vertex_Minimal> vertices;
    vertices.push_back({  { 0.0,-0.5,0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  { 0.5,0.5,0.0  },{ 0.0,-0.5 } });
    vertices.push_back({  { -0.5,0.5,0.0 },{ 0.0,-0.5 } });

    const size_t vertexBufferSize = sizeof(Vertex_Minimal)  * vertices.size();
    Buffer stagingBuffer = bufferManager.CreateBuffer(vertexBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.CopyData(vertices.data(),vertexBufferSize);
    Buffer vertexBuffer = bufferManager.CreateBuffer(stagingBuffer.bufferCreateInfo.size,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
    bufferManager.CopyBuffer(&stagingBuffer,&vertexBuffer);

    Descriptor descriptor(context); // This should be destroyed after pipeline and pipeline layout
    descriptor.CreateMemoryPool();
    descriptor.CreateGlobalSetLayout();
    descriptor.AllocateGlobalSet();
    descriptor.UpdateGlobalSet(vertexBuffer,framebuffer);

   //Model model("Webley.fbx");

    GraphicsPipeline forwardLightingGraphicsPipeline =  GraphicsPipelineBuilder()
                                            .InputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                            .VertexInput(GraphicsPipelineBuilder::VertexInputTypes::VERTEX_FORMAT_MINIMAL)
                                            .SetDynamicState(GraphicsPipelineBuilder::DynamicStateType::ViewportAndScissor)
                                            .SetVertexShader(triangleShader.GetVertexShaderModule())
                                            .SetFragmentShader(triangleShader.GetFragmentShaderModule())
                                            .SetDepth(false,false)
                                            .SetRasterizationCullMode(VK_CULL_MODE_NONE)
                                            .SetMultiSampling(renderer.GetSamplesCount(),false,false)
                                            .AddColorblendAttachment(4,GraphicsPipelineBuilder::BlendingType::OPAQUE,1) // 4 attachments to write directly to final image
                                            .AddDescriptorSet(descriptor.GetGlobalSetLayout())
                                            .Build(renderpass.GetHandle(),0,context);

    GraphicsPipeline forwardPostprocessingGraphicsPipeline =  GraphicsPipelineBuilder()
                                            .InputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                            .VertexInput(GraphicsPipelineBuilder::VertexInputTypes::VERTEX_FORMAT_NONE)
                                            .SetDynamicState(GraphicsPipelineBuilder::DynamicStateType::ViewportAndScissor)
                                            .SetVertexShader(postprocessingShader.GetVertexShaderModule())
                                            .SetFragmentShader(postprocessingShader.GetFragmentShaderModule())
                                            .SetDepth(false,false)
                                            .SetRasterizationCullMode(VK_CULL_MODE_NONE)
                                            .AddColorblendAttachment(4,GraphicsPipelineBuilder::BlendingType::OPAQUE,1) // 4 attachments to write directly to final image
                                            .AddDescriptorSet(descriptor.GetGlobalSetLayout())
                                            .Build(renderpass.GetHandle(),1,context);




    GameTime time;


    while (!window.ShouldCloseWindow())
    {
        time.Update();

        renderer.AcquireImage();

        renderer.ResetGraphicsCommandBuffer();

        renderer.BeginGraphicsCommandBuffer();
        renderer.SetViewportScissor();

        renderer.BeginRenderpass(renderpass.GetHandle(),framebuffer.GetCurrentFramebuffer(renderer.GetCurrentImageIndex()));

        renderer.BindDescriptorSet(forwardLightingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet());// This will only work if both pipelines have same layout

        VkDeviceSize offset = 0;
        renderer.BindGraphicsPipeline(forwardLightingGraphicsPipeline.GetPipeline());
        //renderer.BindDescriptorSet(forwardLightingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet()); // bind once
        renderer.BindVertexBuffer(0,1,vertexBuffer.handle,offset);

        renderer.Draw(vertices.size(),1,0,0);

        renderer.NextSubpass();

        renderer.BindGraphicsPipeline(forwardPostprocessingGraphicsPipeline.GetPipeline());
        //renderer.BindDescriptorSet(forwardPostprocessingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet()); // bind twice if layout is different

        renderer.Draw(3,1,0,0);

        renderer.EndRenderpass();
        renderer.EndGraphicsCommandBuffer();

        renderer.SubmitGraphicsQueue();
        renderer.PresentImage();
        renderer.AdvanceFrame();

    }
}