#include "Headers.h"

int main(int argc,char* argv[])
{
    Window window("Ajax Town",640,480);
    Context context(window);
    Swapchain swapchain(context);
    Renderpass renderpass(context);
    ImageManager imageManager(context);
    BufferManager bufferManager(context);
    Framebuffer framebuffer(context,swapchain,imageManager,renderpass);
    Renderer renderer(context,swapchain,swapchain.imageCount);
    Shader triangleShader(context,"Triangle");
    GraphicsPipeline graphicsPipeline =  GraphicsPipelineBuilder()
                                        .InputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                        .VertexInput(GraphicsPipelineBuilder::VertexInputTypes::VERTEX_FORMAT_MINIMAL)
                                        .SetDynamicState(GraphicsPipelineBuilder::DynamicStateType::ViewportAndScissor)
                                        .SetVertexShader(triangleShader.GetVertexShaderModule())
                                        .SetFragmentShader(triangleShader.GetFragmentShaderModule())
                                        .SetDepth(false,false)
                                        .SetRasterizationCullMode(VK_CULL_MODE_NONE)
                                        .AddColorblendAttachment(4,GraphicsPipelineBuilder::BlendingType::OPAQUE,4) // 4 attachments to write directly to final image
                                        .Build(renderpass.GetHandle(),0,context);


    std::vector<Vertex_Minimal> vertices;
    vertices.push_back({  { 1.0,0.0,0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  { 0.0,1.0,0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  { 0.0,0.0,1.0 },{ 0.0,-0.5 } });

    const size_t vertexBufferSize = sizeof(Vertex_Minimal)  * vertices.size();
    Buffer stagingBuffer = bufferManager.CreateBuffer(vertexBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer.CopyData(vertices.data(),vertexBufferSize);
    Buffer vertexBuffer = bufferManager.CreateBuffer(stagingBuffer.bufferCreateInfo.size,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,VMA_MEMORY_USAGE_GPU_ONLY);
    bufferManager.CopyBuffer(&stagingBuffer,&vertexBuffer);
    //Buffer vertexBuffer = bufferManager.CreateBuffer(vertexBufferSize,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,VMA_MEMORY_USAGE_GPU_ONLY);

    GameTime time;
    //while (window.ShouldCloseWindow())
    while (!window.ShouldCloseWindow())
    {
        time.Update();

        renderer.AcquireImage();

        renderer.ResetGraphicsCommandBuffer();

        renderer.BeginGraphicsCommandBuffer();
        renderer.SetViewportScissor();
        renderer.BeginRenderpass(renderpass.GetHandle(),framebuffer.GetCurrentFramebuffer(renderer.GetCurrentImageIndex()));

        renderer.BindGraphicsPipeline(graphicsPipeline.GetPipeline());
        VkDeviceSize offset = 0;
        renderer.BindVertexBuffer(0,1,vertexBuffer.buffer,offset);

        renderer.Draw(vertices.size(),1,0,0);
        renderer.NextSubpass();
        renderer.NextSubpass();

        renderer.EndRenderpass();
        renderer.EndGraphicsCommandBuffer();

        renderer.SubmitGraphicsQueue();
        renderer.PresentImage();
        renderer.AdvanceFrame();
    }
}