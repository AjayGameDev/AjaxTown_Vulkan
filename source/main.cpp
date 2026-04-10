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
    Shader triangleShader(context,"Triangle",ShaderType::vertfrag);
    Shader standardShader(context,"standard",ShaderType::vertfrag);
    Shader postprocessingShader(context,"PostProcessing",ShaderType::vertfrag);
    Shader cullingShader(context,"Culling",ShaderType::comp);

    const uint32_t maxDrawCount = 10000;
/*
    std::vector<Vertex_Minimal> vertices;
    vertices.push_back({  {  0.0,-0.5, 0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  {  0.5, 0.5, 0.0 },{ 0.0,-0.5 } });
    vertices.push_back({  { -0.5, 0.5, 0.0 },{ 0.0,-0.5 } });

    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
*/
    struct Mesh
    {
        uint32_t    indexCount;
        uint32_t    instanceCount;
        uint32_t    indexOffset;
        int32_t     vertexOffset;
        uint32_t    firstInstance;
    };

    Model model_revolver("Webley.obj");
    Model model_shotgun("Shotgun.obj");
    Mesh mesh_revolver,mesh_shotgun;

    std::vector<Vertex_Standard> vertices;
    std::vector<uint32_t> indices;
    std::vector<Mesh> meshes;

    mesh_revolver.indexCount    = model_revolver.GetIndices().size();
    mesh_revolver.indexOffset   = indices.size();
    mesh_revolver.vertexOffset  = vertices.size();
    mesh_revolver.instanceCount = 1;
    mesh_revolver.firstInstance = 0;

    meshes.push_back(mesh_revolver);
    vertices.insert(vertices.end(),model_revolver.GetVertices().begin(),model_revolver.GetVertices().end());
    indices.insert(indices.end(),model_revolver.GetIndices().begin(),model_revolver.GetIndices().end());

    mesh_shotgun.indexCount    = model_shotgun.GetIndices().size();
    mesh_shotgun.indexOffset   = indices.size();
    mesh_shotgun.vertexOffset  = vertices.size();
    mesh_shotgun.instanceCount = 1;
    mesh_shotgun.firstInstance = 0;

    meshes.push_back(mesh_shotgun);
    vertices.insert(vertices.end(),model_shotgun.GetVertices().begin(),model_shotgun.GetVertices().end());
    indices.insert(indices.end(),model_shotgun.GetIndices().begin(),model_shotgun.GetIndices().end());


/*
    uint32_t indexOffset;

    indexOffset = vertices.size();
    for (int i = 0; i < model_revolver.GetIndices().size(); ++i)
    {
        indices.push_back(model_revolver.GetIndices()[i] + indexOffset);
    }
    vertices.insert(vertices.end(),model_revolver.GetVertices().begin(),model_revolver.GetVertices().end());
    mesh_revolver.indexCount = model_revolver.GetIndices().size();
    mesh_revolver.instanceCount = 1;
    mesh_revolver.firstIndex = indexOffset;
    mesh_revolver.vertexOffset = vertices.size();
    mesh_revolver.firstInstance = 0;

    indexOffset = vertices.size();
    for (int i = 0; i < model_shotgun.GetIndices().size(); ++i)
    {
        indices.push_back(model_shotgun.GetIndices()[i] + indexOffset);
    }
    vertices.insert(vertices.end(),model_shotgun.GetVertices().begin(),model_shotgun.GetVertices().end());

    mesh_shotgun.indexCount = model_shotgun.GetIndices().size();
    mesh_shotgun.instanceCount = 1;
    mesh_shotgun.firstIndex = indexOffset;
    mesh_shotgun.vertexOffset = vertices.size();
    mesh_shotgun.firstInstance = 0;
*/

    Camera camera; // main camera
    Transform transform_camera;
    camera.GenerateViewProjectionMatrix(transform_camera);

    std::vector<Transform> transforms;
    Transform transform_revolver,transform_shotgun; // object transform
    transforms.push_back(transform_revolver);
    transforms.push_back(transform_shotgun);



/*
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
*/

    const size_t vertexBufferSize                =    sizeof(Vertex_Standard)  * vertices.size();
    const size_t indexBufferSize                 =    sizeof(uint32_t)  * indices.size();
    const size_t transformsBufferSize            =    sizeof(Transform) * transforms.size();
    const size_t meshesBufferSize                =    sizeof(Mesh) * meshes.size();
    const size_t modelMatricesBufferSize         =    sizeof(Matrix4) * transforms.size();
    constexpr size_t drawCommandsBufferSize      =    sizeof(VkDrawIndexedIndirectCommand) * maxDrawCount;
    constexpr size_t drawCountBufferSize         =    sizeof(uint32_t);

    Buffer buffer_vertex           =   bufferManager.CreateBuffer( vertexBufferSize         , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT    |  VK_BUFFER_USAGE_TRANSFER_DST_BIT                                             ,  VMA_MEMORY_USAGE_GPU_ONLY);
    Buffer buffer_index            =   bufferManager.CreateBuffer( indexBufferSize          , VK_BUFFER_USAGE_INDEX_BUFFER_BIT     |  VK_BUFFER_USAGE_TRANSFER_DST_BIT                                             ,  VMA_MEMORY_USAGE_GPU_ONLY);
    Buffer buffer_drawCommands     =   bufferManager.CreateBuffer( drawCommandsBufferSize   , VK_BUFFER_USAGE_STORAGE_BUFFER_BIT   |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT                                          ,  VMA_MEMORY_USAGE_GPU_ONLY);
    Buffer buffer_drawCount        =   bufferManager.CreateBuffer( drawCountBufferSize      , VK_BUFFER_USAGE_STORAGE_BUFFER_BIT   |  VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT       ,  VMA_MEMORY_USAGE_GPU_ONLY); // dst is used for vkCmdFillBuffer to reset count before starting new frame
    Buffer buffer_modelMatrices    =   bufferManager.CreateBuffer( modelMatricesBufferSize  , VK_BUFFER_USAGE_STORAGE_BUFFER_BIT   |  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT                                    ,  VMA_MEMORY_USAGE_GPU_ONLY);
    Buffer buffer_transforms       =   bufferManager.CreateBuffer( transformsBufferSize     , VK_BUFFER_USAGE_STORAGE_BUFFER_BIT   |  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,  VMA_MEMORY_USAGE_GPU_ONLY);
    Buffer buffer_meshes           =   bufferManager.CreateBuffer( meshesBufferSize         , VK_BUFFER_USAGE_STORAGE_BUFFER_BIT   |  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,  VMA_MEMORY_USAGE_GPU_ONLY);

    // transfer data to buffers

    Buffer stagingBuffer_vertex = bufferManager.CreateBuffer(vertexBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer_vertex.CopyData(vertices.data(),vertexBufferSize);
    bufferManager.CopyBuffer(&stagingBuffer_vertex,&buffer_vertex);

    Buffer stagingBuffer_index = bufferManager.CreateBuffer(indexBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer_index.CopyData(indices.data(),indexBufferSize);
    bufferManager.CopyBuffer(&stagingBuffer_index,&buffer_index);

    Buffer stagingBuffer_transforms = bufferManager.CreateBuffer(transformsBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer_transforms.CopyData(transforms.data(),transformsBufferSize);
    bufferManager.CopyBuffer(&stagingBuffer_transforms,&buffer_transforms);

    Buffer stagingBuffer_meshes   = bufferManager.CreateBuffer(meshesBufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer_meshes.CopyData(meshes.data(),meshesBufferSize);
    bufferManager.CopyBuffer(&stagingBuffer_meshes,&buffer_meshes);



    // Descriptor Management
    Descriptor descriptor(context); // This should be destroyed after pipeline and pipeline layout
    descriptor.CreateMemoryPool();
    descriptor.CreateGlobalSetLayout();
    descriptor.CreateComputeSetLayout();
    descriptor.AllocateGlobalSet();
    descriptor.AllocateComputeSet();
    descriptor.UpdateGlobalSet(framebuffer);
    descriptor.UpdateComputeSet(buffer_drawCommands,buffer_drawCount);

    // 128 bytes are widely supported but some devices support upto 256 bytes so check hardware capabilities
    struct PushConstantData_Compute
    {
        uint64_t transformsAddress;
        uint64_t modelMatricesAddress;
        uint64_t meshesAddress;
    }pushConstantData_compute;

    pushConstantData_compute.transformsAddress    = buffer_transforms.GetAddress();
    pushConstantData_compute.modelMatricesAddress = buffer_modelMatrices.GetAddress();
    pushConstantData_compute.meshesAddress        = buffer_meshes.GetAddress();

    PushConstant pushConstant_compute(VK_SHADER_STAGE_COMPUTE_BIT,0,sizeof(PushConstantData_Compute));

    struct PushConstantData_ForwardRendering
    {
        Matrix4  viewProjectionMatrix;
        uint64_t modelMatricesAddress;
    }pushConstantData_forwardRendering;

    memcpy(pushConstantData_forwardRendering.viewProjectionMatrix,camera.viewProjectionMatrix,sizeof(Matrix4)); // bcz it is decalred as typedef float model[4][4] instead of a struct Matrix4 { float[4][4] model };
    pushConstantData_forwardRendering.modelMatricesAddress = buffer_modelMatrices.GetAddress();

    PushConstant pushConstant_forwardRendering(VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(PushConstantData_ForwardRendering));

    GraphicsPipeline forwardLightingGraphicsPipeline =  GraphicsPipelineBuilder()
                                            .InputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                                            .VertexInput(GraphicsPipelineBuilder::VertexInputTypes::VERTEX_FORMAT_STANDARD)
                                            .SetDynamicState(GraphicsPipelineBuilder::DynamicStateType::ViewportAndScissor)
                                            .SetVertexShader(standardShader.GetVertexShaderModule())
                                            .SetFragmentShader(standardShader.GetFragmentShaderModule())
                                            .SetDepth(true,true,VK_COMPARE_OP_GREATER)
                                            .SetRasterizationCullMode(VK_CULL_MODE_BACK_BIT)
                                            .SetMultiSampling(renderer.GetSamplesCount(),false,false)
                                            .AddColorblendAttachment(4,GraphicsPipelineBuilder::BlendingType::OPAQUE,1) // 4 attachments to write directly to final image
                                            .AddPushConstant(pushConstant_forwardRendering.GetHandle())
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
                                             .AddPushConstant(pushConstant_compute.GetHandle())
                                             .AddDescriptorSetLayout(descriptor.GetComputeSetLayout())
                                             .Build(context);

    GameTime time;


    while (!window.ShouldCloseWindow())
    {
        time.Update();

        renderer.AcquireImage();

        renderer.ResetDrawCountBuffer(buffer_drawCount.GetHandle());

        //renderer.ResetComputeCommandBuffer();  // not necessary as we are using VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        renderer.BeginComputeCommandBuffer();

        renderer.BindComputePipeline(cullingComputePipeline.GetPipeline()); // Bind compute pipeline
        renderer.BindDescriptorSetCompute(cullingComputePipeline.GetPipelineLayout(), descriptor.GetComputeSet()); // Bind compute descriptor set
        renderer.PushConstant_compute(cullingComputePipeline.GetPipelineLayout(),VK_SHADER_STAGE_COMPUTE_BIT,0,sizeof(PushConstantData_Compute),&pushConstantData_compute); // push constant data
        renderer.DispatchComputeShader(1, 1, 1); // dispatch compute shader
        renderer.ComputeToIndirectBarrier(buffer_drawCommands.GetHandle()); // it is a command so should be recorded within command buffer

        renderer.EndComputeCommandBuffer();

        renderer.SubmitComputeQueue();

        //renderer.ResetGraphicsCommandBuffer(); // not necessary as we are using VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT

        renderer.BeginGraphicsCommandBuffer();
        renderer.SetViewportScissor();

        renderer.BeginRenderpass(renderpass.GetHandle(), framebuffer.GetCurrentFramebuffer(renderer.GetCurrentImageIndex()));

        renderer.BindDescriptorSetGraphics(forwardLightingGraphicsPipeline.GetPipelineLayout(), descriptor.GetGlobalSet()); // This will only work if both pipelines have same layout


        VkDeviceSize offset = 0;
        renderer.BindGraphicsPipeline(forwardLightingGraphicsPipeline.GetPipeline());
        //renderer.BindDescriptorSet(forwardLightingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet()); // bind once
        renderer.BindVertexBuffer(0, 1, buffer_vertex.handle, offset);
        renderer.BindIndexBuffer(buffer_index.handle, offset, VK_INDEX_TYPE_UINT32);

        renderer.PushConstant_graphics(forwardLightingGraphicsPipeline.GetPipelineLayout(),VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(PushConstantData_ForwardRendering),&pushConstantData_forwardRendering); // push constant data
        //renderer.Draw(vertices.size(),1,0,0);
        //renderer.DrawIndirect(indirectBuffer.GetHandle(),0,1,sizeof(VkDrawIndexedIndirectCommand));

        renderer.DrawIndexedIndirectCount(buffer_drawCommands.GetHandle(), 0, buffer_drawCount.GetHandle(), 0, maxDrawCount, sizeof(VkDrawIndexedIndirectCommand)); // max draw might change

        renderer.NextSubpass();

        renderer.BindGraphicsPipeline(forwardPostprocessingGraphicsPipeline.GetPipeline());
        //renderer.BindDescriptorSet(forwardPostprocessingGraphicsPipeline.GetPipelineLayout(),descriptor.GetGlobalSet()); // bind twice if layout is different

        renderer.Draw(3, 1, 0, 0); // drawing fullscreen triangle instead of a quad

        renderer.EndRenderpass();
        renderer.EndGraphicsCommandBuffer();

        renderer.SubmitGraphicsQueue();
        renderer.PresentImage();
        renderer.AdvanceFrame();
    }
}

