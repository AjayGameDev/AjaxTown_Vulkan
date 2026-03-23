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

    // Shader handling
    Shader triangleShader(context,"Triangle",ShaderType::vert);
    Shader postprocessingShader(context,"PostProcessing",ShaderType::vert);
    Shader cullingShader(context,"Culling",ShaderType::comp);

    std::vector<Vertex_Minimal> vertices;
    vertices.push_back({  {  0.0,-0.5, 0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  {  0.5, 0.5, 0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  { -0.5, 0.5, 0.0 },{ 0.0,-0.5 } });

    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    const size_t vertexBufferSize = sizeof(Vertex_Minimal)  * vertices.size();
    Buffer stagingBuffer = bufferManager.CreateBuffer(vertexBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.CopyData(vertices.data(),vertexBufferSize);
    Buffer vertexBuffer = bufferManager.CreateBuffer(stagingBuffer.bufferCreateInfo.size,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
    bufferManager.CopyBuffer(&stagingBuffer,&vertexBuffer);

    const size_t indexBufferSize = sizeof(uint32_t)  * indices.size();
    Buffer stagingIndexBuffer = bufferManager.CreateBuffer(indexBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingIndexBuffer.CopyData(indices.data(),indexBufferSize);
    Buffer indexBuffer = bufferManager.CreateBuffer(stagingIndexBuffer.bufferCreateInfo.size,VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
    bufferManager.CopyBuffer(&stagingIndexBuffer,&indexBuffer);

    Buffer indirectBuffer = bufferManager.CreateBuffer(sizeof(VkDrawIndexedIndirectCommand) * 10000,VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
    Buffer countBuffer    = bufferManager.CreateBuffer(sizeof(uint32_t),VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,VMA_MEMORY_USAGE_GPU_ONLY);

    // Descriptor Management
    Descriptor descriptor(context); // This should be destroyed after pipeline and pipeline layout
    descriptor.CreateMemoryPool();
    descriptor.CreateGlobalSetLayout();
    descriptor.CreateComputeSetLayout();
    descriptor.AllocateGlobalSet();
    descriptor.AllocateComputeSet();
    descriptor.UpdateGlobalSet(framebuffer);
    descriptor.UpdateComputeSet(indirectBuffer,countBuffer);
    float verticalFov = 60;
    float f = 1.0f  / tanf(verticalFov * .5f);
   Model model("Webley.obj");

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


    ComputePipeline cullingComputePipeline = ComputePipelineBuilder()
                                             .SetComputeShader(cullingShader.GetComputeShaderModule())
                                             .AddDescriptorSetLayout(descriptor.GetComputeSetLayout())
                                             .Build(context);


    GameTime time;


    while (!window.ShouldCloseWindow())
    {
        time.Update();

        renderer.AcquireImage();

        //renderer.ResetComputeCommandBuffer();  // not necessary as we are using VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        renderer.BeginComputeCommandBuffer();

        renderer.BindComputePipeline(cullingComputePipeline.GetPipeline()); // Bind compute pipeline
        renderer.BindDescriptorSetCompute(cullingComputePipeline.GetPipelineLayout(),descriptor.GetComputeSet()); // Bind compute descriptor set
        renderer.DispatchComputeShader(1,1,1); // dispatch compute shader
        renderer.ComputeToIndirectBarrier(indirectBuffer.GetHandle()); // it is a command so should be recorded within command buffer

        renderer.EndComputeCommandBuffer();

        renderer.SubmitComputeQueue();

        //renderer.ResetGraphicsCommandBuffer(); // not necessary as we are using VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT

        renderer.BeginGraphicsCommandBuffer();
        renderer.SetViewportScissor();

        renderer.BeginRenderpass(renderpass.GetHandle(),framebuffer.GetCurrentFramebuffer(renderer.GetCurrentImageIndex()));

        renderer.BindDescriptorSetGraphics(forwardLightingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet());// This will only work if both pipelines have same layout


        VkDeviceSize offset = 0;
        renderer.BindGraphicsPipeline(forwardLightingGraphicsPipeline.GetPipeline());
        //renderer.BindDescriptorSet(forwardLightingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet()); // bind once
        renderer.BindVertexBuffer(0,1,vertexBuffer.handle,offset);
        renderer.BindIndexBuffer(indexBuffer.handle,offset,VK_INDEX_TYPE_UINT32);

        //renderer.Draw(vertices.size(),1,0,0);
        //renderer.DrawIndirect(indirectBuffer.GetHandle(),0,1,sizeof(VkDrawIndexedIndirectCommand));

        renderer.DrawIndexedIndirectCount(indirectBuffer.GetHandle(),0,countBuffer.GetHandle(),0,10000,sizeof(VkDrawIndexedIndirectCommand)); // max draw might change

        renderer.NextSubpass();

        renderer.BindGraphicsPipeline(forwardPostprocessingGraphicsPipeline.GetPipeline());
        //renderer.BindDescriptorSet(forwardPostprocessingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet()); // bind twice if layout is different

        renderer.Draw(3,1,0,0); // drawing fullscreen triangle instead of a quad

        renderer.EndRenderpass();
        renderer.EndGraphicsCommandBuffer();

        renderer.SubmitGraphicsQueue();
        renderer.PresentImage();
        renderer.AdvanceFrame();

    }
}