#pragma once

#include "Model.h"
#include "GraphicsPipeline.h"

class GraphicsPipelineBuilder
{

#pragma region fields

    private:
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
            VkPipelineVertexInputStateCreateInfo   vertexInputInfo{};
            VkPipelineViewportStateCreateInfo      viewportInfo{};
            VkPipelineDynamicStateCreateInfo       dynamicStatesInfo{};
            VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
            VkPipelineMultisampleStateCreateInfo   multisamplingInfo{};
            VkPipelineColorBlendStateCreateInfo    colorblendInfo{};
            VkPipelineDepthStencilStateCreateInfo  depthStencilInfo{};
            VkPipelineLayoutCreateInfo             pipelineLayoutInfo{};
            VkGraphicsPipelineCreateInfo           graphicsPipelineInfo{};

            std::vector<VkVertexInputBindingDescription>      vertexBindings;
            std::vector<VkVertexInputAttributeDescription>    vertexAttributes;
            std::vector<VkDynamicState>                       dynamicStates;
            std::vector<VkPipelineShaderStageCreateInfo>      shaderStages;
            std::vector<VkPipelineColorBlendAttachmentState>  colorblendAttachments;
            std::vector<VkPushConstantRange>                  pushConstants;
            std::vector<VkDescriptorSetLayout>                descriptorSets;


            VkPipeline       graphicsPipeline = nullptr;
            VkPipelineLayout pipelineLayout{};

#pragma endregion

#pragma region enums

    public:
            enum class VertexInputTypes
            {
                VERTEX_FORMAT_STANDARD,
                VERTEX_FORMAT_SKINNED,
                VERTEX_FORMAT_MINIMAL,
                VERTEX_FORMAT_NONE
            };

            enum class BlendingType
            {
                OPAQUE,
                ALPHA,
                ADDITIVE
            };

            enum class StencilType
            {
                None, // disable stencil test, default setting
                Read, // only read, don't write
                Write, // write reference value [for portals, mirrors], great for masking
                IncrementalWrap, // Good for outline effect
                DecrementalWrap,  // same here
                IncrementalClamp,
                DecrementalClamp
            };

            enum class DynamicStateType
            {
                ViewportAndScissor,
                StencilReference // to be done
            };

#pragma endregion

#pragma region Input Assembly

            //                                        Input Assembly

            void ResetInputAssembly()
            {
                inputAssemblyInfo.sType                     =   VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssemblyInfo.topology                  =   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;    // use line for debugging or point for point cloud data
                inputAssemblyInfo.primitiveRestartEnable    =   false; // used for triangle strips

            }

            GraphicsPipelineBuilder& InputAssembly(VkPrimitiveTopology topology,bool restartPrimitive = false)
            {
                inputAssemblyInfo.topology                  =   topology;
                inputAssemblyInfo.primitiveRestartEnable    =   restartPrimitive;
                return *this;
            }

#pragma endregion

#pragma region Vertex Input

            //                                        Vertex Input


            void ResetVertexInput()
            {
                vertexInputInfo.sType                               =   VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputInfo.vertexBindingDescriptionCount       =   0; // vertex binding
                vertexInputInfo.pVertexBindingDescriptions          =   nullptr;
                vertexInputInfo.vertexAttributeDescriptionCount     =   0; // vertex attributes
                vertexInputInfo.pVertexAttributeDescriptions        =   nullptr;

                vertexBindings.clear();
                vertexAttributes.clear();
            }

            GraphicsPipelineBuilder& VertexInput(VertexInputTypes vertexInputType)
            {
                if (vertexInputType == VertexInputTypes::VERTEX_FORMAT_STANDARD)
                {
                    VkVertexInputBindingDescription vertexBindingDescription_first =
                    {
                        0, // binding
                        sizeof(Vertex_Standard), // stride
                        VK_VERTEX_INPUT_RATE_VERTEX // instance or normal
                    };

                    vertexBindings.push_back(vertexBindingDescription_first);


                    VkVertexInputAttributeDescription vertexPositionAttribute   =  { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Standard,position) }; // location binding format offset
                    VkVertexInputAttributeDescription vertexNormalAttribute     =  { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Standard,normal) };
                    VkVertexInputAttributeDescription vertexUVAttribute         =  { 2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex_Standard,uv) };
                    VkVertexInputAttributeDescription vertexTangentAttribute    =  { 3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Standard,tangent) };
                    //VkVertexInputAttributeDescription vertexBitangentAttribute  =  { 4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Standard,bitangent) };

                    vertexAttributes.push_back(vertexPositionAttribute);
                    vertexAttributes.push_back(vertexNormalAttribute);
                    vertexAttributes.push_back(vertexUVAttribute);
                    vertexAttributes.push_back(vertexTangentAttribute);
                    //vertexAttributes.push_back(vertexBitangentAttribute);

                }
                else if (vertexInputType == VertexInputTypes::VERTEX_FORMAT_MINIMAL)
                {
                    VkVertexInputBindingDescription vertexBindingDescription_first =
                    {
                        0, // binding
                        sizeof(Vertex_Minimal), // stride
                        VK_VERTEX_INPUT_RATE_VERTEX // instance or normal
                    };

                    vertexBindings.push_back(vertexBindingDescription_first);


                    VkVertexInputAttributeDescription vertexPositionAttribute   =  { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Minimal,position) }; // location binding format offset
                    VkVertexInputAttributeDescription vertexUVAttribute         =  { 1, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(Vertex_Minimal,uv) };

                    vertexAttributes.push_back(vertexPositionAttribute);
                    vertexAttributes.push_back(vertexUVAttribute);
                }
                else if (vertexInputType == VertexInputTypes::VERTEX_FORMAT_SKINNED)
                {
                    // work in progress

                }
                else if (vertexInputType == VertexInputTypes::VERTEX_FORMAT_NONE)
                {

                }

                //vertexInputInfo.vertexBindingDescriptionCount    =  vertexBindings.size();
                //vertexInputInfo.pVertexBindingDescriptions       =  vertexBindings.data();
                //vertexInputInfo.vertexAttributeDescriptionCount  =  vertexAttributes.size();
                //vertexInputInfo.pVertexAttributeDescriptions     =  vertexAttributes.data();

                return *this;
            }

            void BuildVertexInput()
            {
                vertexInputInfo.vertexBindingDescriptionCount       =   static_cast<uint32_t>(vertexBindings.size()); // vertex binding
                vertexInputInfo.pVertexBindingDescriptions          =   vertexBindings.data();
                vertexInputInfo.vertexAttributeDescriptionCount     =   static_cast<uint32_t>(vertexAttributes.size()); // vertex attributes
                vertexInputInfo.pVertexAttributeDescriptions        =   vertexAttributes.data();
            }

#pragma endregion

#pragma region Viewport


            void ResetViewport()
            {
                viewportInfo.sType          =   VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportInfo.viewportCount  =   1;
                viewportInfo.pViewports     =   nullptr; // dynamic viewport
                viewportInfo.scissorCount   =   1;
                viewportInfo.pScissors      =   nullptr; // dynamic scissor
            }


#pragma endregion

#pragma region Dynamic States

            //                                 Dynamic States

            void ResetDynamicStates()
            {
                dynamicStates.clear();
            }



            GraphicsPipelineBuilder& SetDynamicState(DynamicStateType dynamicStateType)
            {
                switch (dynamicStateType)
                {
                    case DynamicStateType::ViewportAndScissor:
                        dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
                        dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
                        break;

                    case DynamicStateType::StencilReference:
                        // to be done
                        break;
                }


                return *this;
            }

            void BuildDynamicStates()
            {
                dynamicStatesInfo.sType              =  VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStatesInfo.dynamicStateCount  =  dynamicStates.size();
                dynamicStatesInfo.pDynamicStates     =  dynamicStates.data();
            }

#pragma endregion

#pragma region Shader Stages

            //                              Shader Stages

            void ResetShaderStages()
            {
                shaderStages.clear();
            }

            GraphicsPipelineBuilder& SetVertexShader(VkShaderModule vertexShaderModule)
            {
                VkPipelineShaderStageCreateInfo vertexShaderStage{};

                vertexShaderStage.sType   =  VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertexShaderStage.stage   =  VK_SHADER_STAGE_VERTEX_BIT; // type of shader
                vertexShaderStage.module  =  vertexShaderModule;        // compiled spir-v shader
                vertexShaderStage.pName   =  "main";                   // name of the entry point function in the shader

                shaderStages.push_back(vertexShaderStage);
                return *this;
            }

            GraphicsPipelineBuilder& SetFragmentShader(VkShaderModule fragmentShaderModule)
            {
                VkPipelineShaderStageCreateInfo fragmentShaderStage{};

                fragmentShaderStage.sType   =  VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                fragmentShaderStage.stage   =  VK_SHADER_STAGE_FRAGMENT_BIT; // type of shader
                fragmentShaderStage.module  =  fragmentShaderModule; // compiled spir-v shader
                fragmentShaderStage.pName   =  "main"; // name of the entry point function in the shader

                shaderStages.push_back(fragmentShaderStage);
                return *this;
            }

            GraphicsPipelineBuilder& SetComputeShader(VkShaderModule computehaderModule)
            {
                VkPipelineShaderStageCreateInfo computeShaderStage{};

                computeShaderStage.sType   =  VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                computeShaderStage.stage   =  VK_SHADER_STAGE_COMPUTE_BIT; // type of shader
                computeShaderStage.module  =  computehaderModule; // compiled spir-v shader
                computeShaderStage.pName   =  "main"; // name of the entry point function in the shader

                shaderStages.push_back(computeShaderStage);
                return *this;
            }

#pragma endregion

#pragma region Color Blend State

            //                                              Color Blend State

            void ResetColorBlend()
            {
                colorblendAttachments.clear();
                colorblendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            }

            GraphicsPipelineBuilder& AddColorblendAttachment(int numberOfColorChannels, BlendingType blendtype, int numberOfAttachements = 1)
            {

                for (int i = 0; i < numberOfAttachements; ++i)
                {
                    VkPipelineColorBlendAttachmentState colorblendAttachmentState{};
                    if (numberOfColorChannels == 4)
                        colorblendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                    else if (numberOfColorChannels == 3)
                        colorblendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;

                    switch (blendtype)
                    {
                        case BlendingType::OPAQUE :

                            colorblendAttachmentState.blendEnable = false;
                            // for opaque blending is disabled so these parameters don't matter
                              break;

                        case BlendingType::ALPHA :

                            colorblendAttachmentState.blendEnable = true;
                            // finalColor = srcColor * srcAlpha + DstColor * (1 - srcAlpha)
                            // finalAlpha = srcAlpha [Alpha part will only work if referenced attachment in renderpass has alpha in format] [For R8G8B8_UNORM alpha will be ignored] [R8G8B8A8_UNORM alpha will be used]

                            colorblendAttachmentState.srcColorBlendFactor   =  VK_BLEND_FACTOR_SRC_ALPHA;
                            colorblendAttachmentState.dstColorBlendFactor   =  VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                            colorblendAttachmentState.colorBlendOp          =  VK_BLEND_OP_ADD;
                            colorblendAttachmentState.srcAlphaBlendFactor   =  VK_BLEND_FACTOR_ONE;
                            colorblendAttachmentState.dstAlphaBlendFactor   =  VK_BLEND_FACTOR_ZERO;
                            colorblendAttachmentState.alphaBlendOp          =  VK_BLEND_OP_ADD;

                            break;

                        case BlendingType::ADDITIVE :

                            colorblendAttachmentState.blendEnable = true;
                            // finalColor = srcColor * srcAlpha + DstColor * 1
                            // finalAlpha = srcAlpha + dstAlpha [Alpha part will only work if referenced attachment in renderpass has alpha in format] [For R8G8B8_UNORM alpha will be ignored] [R8G8B8A8_UNORM alpha will be used]

                            colorblendAttachmentState.srcColorBlendFactor   =   VK_BLEND_FACTOR_SRC_ALPHA;
                            colorblendAttachmentState.dstColorBlendFactor   =   VK_BLEND_FACTOR_ONE;
                            colorblendAttachmentState.colorBlendOp          =   VK_BLEND_OP_ADD;
                            colorblendAttachmentState.srcAlphaBlendFactor   =   VK_BLEND_FACTOR_ONE;
                            colorblendAttachmentState.dstAlphaBlendFactor   =   VK_BLEND_FACTOR_ONE;
                            colorblendAttachmentState.alphaBlendOp          =   VK_BLEND_OP_ADD;

                            break;
                    }

                    colorblendAttachments.push_back(colorblendAttachmentState);
                }



                return *this;
            }

            GraphicsPipelineBuilder& BuildColorBlendState()
            {
                colorblendInfo.logicOpEnable      =  false; // for opaque,alpha,additive it's not needed
                colorblendInfo.logicOp            =  VK_LOGIC_OP_COPY; // doesn't matter as logicOp is false
                colorblendInfo.attachmentCount    =  static_cast<uint32_t>(colorblendAttachments.size());
                colorblendInfo.pAttachments       =  colorblendAttachments.data();
                colorblendInfo.blendConstants[0]  =  0;    // Not used mostly
                colorblendInfo.blendConstants[1]  =  0;   // Not used mostly
                colorblendInfo.blendConstants[2]  =  0;  // Not used mostly
                colorblendInfo.blendConstants[3]  =  0; // Not used mostly

                return *this;
            }

#pragma endregion

#pragma region Rasterization

            //                                      Rasterization

            void ResetRasterization()
            {
                rasterizationInfo.sType                     =   VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizationInfo.cullMode                  =   VK_CULL_MODE_BACK_BIT;
                rasterizationInfo.frontFace                 =   VK_FRONT_FACE_COUNTER_CLOCKWISE;
                rasterizationInfo.polygonMode               =   VK_POLYGON_MODE_FILL;
                rasterizationInfo.depthClampEnable          =   false;
                rasterizationInfo.depthBiasEnable           =   false;
                rasterizationInfo.lineWidth                 =   1.0f;
                rasterizationInfo.rasterizerDiscardEnable   =   false;
            }

            GraphicsPipelineBuilder& SetRasterizationCullMode(VkCullModeFlags cullmode)
            {
                rasterizationInfo.cullMode = cullmode;
                return *this;
            }

            GraphicsPipelineBuilder& SetRasterizationPolygonMode(bool depthClampEnable = false,bool depthBiasEnable = false,float depthBiasConstantFactor = 0, float depthBiasSlopeFactor = 0,float depthBiasClamp = 0)
            {
                rasterizationInfo.depthClampEnable         =  depthClampEnable;
                rasterizationInfo.depthBiasEnable          =  depthBiasEnable;
                rasterizationInfo.depthBiasConstantFactor  =  depthBiasConstantFactor;
                rasterizationInfo.depthBiasSlopeFactor     =  depthBiasSlopeFactor;
                rasterizationInfo.depthBiasClamp           =  depthBiasClamp;

                return *this;
            }

            GraphicsPipelineBuilder& SetRasterizationDepthMode(VkPolygonMode polygonmode)
            {
                rasterizationInfo.polygonMode = polygonmode;
                return *this;
            }

#pragma endregion

#pragma region Multisampling

            //                              MultiSampling

            void ResetMultisampling()
            {
                multisamplingInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisamplingInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
                multisamplingInfo.sampleShadingEnable   = false;
                multisamplingInfo.minSampleShading      = 1.0f;
                multisamplingInfo.alphaToCoverageEnable = false;
                multisamplingInfo.alphaToOneEnable      = false;

            }

            GraphicsPipelineBuilder& SetMultiSampling(VkSampleCountFlagBits samples,bool alphaToCoverageEnable,bool alphaToOneEnable,bool sampleShading = false,float minSampleShading = 1.0f)
            {
                multisamplingInfo.rasterizationSamples  = samples;
                multisamplingInfo.sampleShadingEnable   = sampleShading;
                multisamplingInfo.minSampleShading      = minSampleShading;
                multisamplingInfo.alphaToCoverageEnable = alphaToCoverageEnable;
                multisamplingInfo.alphaToOneEnable      = alphaToOneEnable;

                return *this;
            }
            void BuildMultisampling()
            {
                // to be done, currently it's off so might not be needed as of now
            }

#pragma endregion

#pragma region Depth & Stencil

            //                                  Depth & Stencil

            void ResetDepthStencilState()
            {
                depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

                // depth
                depthStencilInfo.depthTestEnable         =    true;
                depthStencilInfo.depthWriteEnable        =    true;
                depthStencilInfo.depthCompareOp          =    VK_COMPARE_OP_LESS;
                depthStencilInfo.depthBoundsTestEnable   =    false;
                depthStencilInfo.minDepthBounds          =    0.0f;
                depthStencilInfo.maxDepthBounds          =    1.0f;

                // stencil
                depthStencilInfo.stencilTestEnable       =    false;
                depthStencilInfo.back                    =    {};  // for front face of geometry
                depthStencilInfo.front                   =    {}; //  for back face of geometry
            }

            GraphicsPipelineBuilder& SetDepth(bool depthTest,bool depthWrite,VkCompareOp compareOp = VK_COMPARE_OP_LESS,bool depthBounds = false,float minDepthBound = 0.0f,float maxDepthBound = 1.0f )
            {
                depthStencilInfo.depthTestEnable         =    depthTest;
                depthStencilInfo.depthWriteEnable        =    depthWrite;
                depthStencilInfo.depthCompareOp          =    compareOp;
                depthStencilInfo.depthBoundsTestEnable   =    depthBounds;
                depthStencilInfo.minDepthBounds          =    minDepthBound;
                depthStencilInfo.maxDepthBounds          =    maxDepthBound;

                return *this;
            }

            GraphicsPipelineBuilder& SetStencil(bool stencilTest, VkStencilOpState& front, VkStencilOpState& back)
            {
                // it's better to set reference value through vkCmdSetStencilReference(cmd, VK_STENCIL_FACE_FRONT_AND_BACK, stencilRefID);
                // for that we need to add it to dynamic components list
                depthStencilInfo.stencilTestEnable   =  stencilTest;
                depthStencilInfo.front               =  front;
                depthStencilInfo.back                =  back;

                return *this;
            }

            GraphicsPipelineBuilder& SetStencil(StencilType stencilType)
            {
                // it's better to set reference value through vkCmdSetStencilReference(cmd, VK_STENCIL_FACE_FRONT_AND_BACK, stencilRefID);
                // for that we need to add it to dynamic state list otherwise this reference value will be locked at pipeline creation

                switch (stencilType)
                {
                    case StencilType::None :
                        depthStencilInfo.stencilTestEnable = false;
                        break;

                    case StencilType::Read :
                        depthStencilInfo.stencilTestEnable  =  true;
                        depthStencilInfo.front.failOp       =  VK_STENCIL_OP_KEEP;     // Don't change the value in the stencil buffer
                        depthStencilInfo.front.passOp       =  VK_STENCIL_OP_KEEP;    // Don't change the value in the stencil buffer
                        depthStencilInfo.front.depthFailOp  =  VK_STENCIL_OP_KEEP;   // Don't change the value in the stencil buffer
                        depthStencilInfo.front.compareOp    =  VK_COMPARE_OP_EQUAL; // Pass only if [ referenceValue == stencilBufferValue ]
                        depthStencilInfo.front.compareMask  =  0xFF;               // Read all bits [ 0xFF = 11111111 = all bits enabled ]
                        depthStencilInfo.front.writeMask    =  0x00;              // Don't write to any bits [ 0x00 = 00000000, so no bits are writable ]
                        depthStencilInfo.front.reference    =  0;                // this will be changed dynamically while rendering objects
                        break;

                    case StencilType::Write :
                        depthStencilInfo.stencilTestEnable  =  true;
                        depthStencilInfo.front.failOp       =  VK_STENCIL_OP_KEEP;        // Don't change the value in the stencil buffer
                        depthStencilInfo.front.passOp       =  VK_STENCIL_OP_REPLACE;    // Replace the value in the stencil buffer
                        depthStencilInfo.front.depthFailOp  =  VK_STENCIL_OP_KEEP;      // Don't change the value in the stencil buffer
                        depthStencilInfo.front.compareOp    =  VK_COMPARE_OP_ALWAYS;   // always pass the test
                        depthStencilInfo.front.compareMask  =  0xFF;                  // Read all bits
                        depthStencilInfo.front.writeMask    =  0xFF;                 // Write to all bits
                        depthStencilInfo.front.reference    =  0;                   // this will be changed dynamically while rendering objects
                        break;

                    case StencilType::IncrementalWrap :
                        depthStencilInfo.stencilTestEnable  =  true;
                        depthStencilInfo.front.failOp       =  VK_STENCIL_OP_KEEP;        // Don't change the value in the stencil buffer
                        depthStencilInfo.front.passOp       =  VK_STENCIL_OP_INCREMENT_AND_WRAP;    // Increment the value in the stencil buffer
                        depthStencilInfo.front.depthFailOp  =  VK_STENCIL_OP_KEEP;      // Don't change the value in the stencil buffer
                        depthStencilInfo.front.compareOp    =  VK_COMPARE_OP_ALWAYS;   // always pass the test
                        depthStencilInfo.front.compareMask  =  0xFF;                  // Read all bits
                        depthStencilInfo.front.writeMask    =  0xFF;                 // Write to all bits
                        depthStencilInfo.front.reference    =  0;                   // this will be changed dynamically while rendering objects
                        break;

                    case StencilType::DecrementalWrap :
                        depthStencilInfo.stencilTestEnable  =  true;
                        depthStencilInfo.front.failOp       =  VK_STENCIL_OP_KEEP;        // Don't change the value in the stencil buffer
                        depthStencilInfo.front.passOp       =  VK_STENCIL_OP_DECREMENT_AND_WRAP;    // Decrement the value in the stencil buffer
                        depthStencilInfo.front.depthFailOp  =  VK_STENCIL_OP_KEEP;      // Don't change the value in the stencil buffer
                        depthStencilInfo.front.compareOp    =  VK_COMPARE_OP_ALWAYS;   // always pass the test
                        depthStencilInfo.front.compareMask  =  0xFF;                  // Read all bits
                        depthStencilInfo.front.writeMask    =  0xFF;                 // Write to all bits
                        depthStencilInfo.front.reference    =  0;                   // this will be changed dynamically while rendering objects
                        break;

                    case StencilType::IncrementalClamp :
                        depthStencilInfo.stencilTestEnable  =  true;
                        depthStencilInfo.front.failOp       =  VK_STENCIL_OP_KEEP;        // Don't change the value in the stencil buffer
                        depthStencilInfo.front.passOp       =  VK_STENCIL_OP_INCREMENT_AND_CLAMP;    // Increment the value in the stencil buffer
                        depthStencilInfo.front.depthFailOp  =  VK_STENCIL_OP_KEEP;      // Don't change the value in the stencil buffer
                        depthStencilInfo.front.compareOp    =  VK_COMPARE_OP_ALWAYS;   // always pass the test
                        depthStencilInfo.front.compareMask  =  0xFF;                  // Read all bits
                        depthStencilInfo.front.writeMask    =  0xFF;                 // Write to all bits
                        depthStencilInfo.front.reference    =  0;                   // this will be changed dynamically while rendering objects
                        break;

                    case StencilType::DecrementalClamp :
                        depthStencilInfo.stencilTestEnable  =  true;
                        depthStencilInfo.front.failOp       =  VK_STENCIL_OP_KEEP;        // Don't change the value in the stencil buffer
                        depthStencilInfo.front.passOp       =  VK_STENCIL_OP_DECREMENT_AND_CLAMP;    // Decrement the value in the stencil buffer
                        depthStencilInfo.front.depthFailOp  =  VK_STENCIL_OP_KEEP;      // Don't change the value in the stencil buffer
                        depthStencilInfo.front.compareOp    =  VK_COMPARE_OP_ALWAYS;   // always pass the test
                        depthStencilInfo.front.compareMask  =  0xFF;                  // Read all bits
                        depthStencilInfo.front.writeMask    =  0xFF;                 // Write to all bits
                        depthStencilInfo.front.reference    =  0;                   // this will be changed dynamically while rendering objects
                        break;
                }

                depthStencilInfo.back = depthStencilInfo.front;

                return *this;
            }

#pragma endregion

#pragma region Pipeline layout

            //                                     Pipeline layout

            void ResetPipelineLayout()
            {
                pipelineLayoutInfo.sType                   =  VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.pushConstantRangeCount  =  0;
                pipelineLayoutInfo.pPushConstantRanges     =  nullptr;
                pipelineLayoutInfo.setLayoutCount          =  0;
                pipelineLayoutInfo.pSetLayouts             =  nullptr;
                pushConstants.clear();
                descriptorSets.clear();
            }

            GraphicsPipelineBuilder& AddPushConstant(VkPushConstantRange pushConstant)
            {
                pushConstants.push_back(pushConstant);
                return *this;
            }

            GraphicsPipelineBuilder& AddDescriptorSet(VkDescriptorSetLayout descriptorSet)
            {
                descriptorSets.push_back(descriptorSet);
                return *this;
            }

            void BuildPipelineLayout(Context& context)
            {
                pipelineLayoutInfo.pushConstantRangeCount  =  static_cast<uint32_t>(pushConstants.size());
                pipelineLayoutInfo.pPushConstantRanges     =  pushConstants.data();
                pipelineLayoutInfo.setLayoutCount          =  static_cast<uint32_t>(descriptorSets.size());
                pipelineLayoutInfo.pSetLayouts             =  descriptorSets.data();

                if (vkCreatePipelineLayout(context.device,&pipelineLayoutInfo,nullptr,&pipelineLayout) != VK_SUCCESS)
                    throw std::runtime_error("\nCan't create graphics pipeline layout!");
            }

#pragma endregion

#pragma region Builder

            //                          Builder

            void Builder(VkRenderPass& renderpass,uint32_t subpass, Context& context)
            {
                BuildPipelineLayout(context);
                BuildColorBlendState();
                BuildDynamicStates();
                BuildMultisampling();
                BuildVertexInput();

                graphicsPipelineInfo.sType                =   VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                graphicsPipelineInfo.stageCount           =   shaderStages.size();
                graphicsPipelineInfo.pStages              =   shaderStages.data();
                graphicsPipelineInfo.pVertexInputState    =   &vertexInputInfo;
                graphicsPipelineInfo.pInputAssemblyState  =   &inputAssemblyInfo;
                graphicsPipelineInfo.pViewportState       =   &viewportInfo;
                graphicsPipelineInfo.pRasterizationState  =   &rasterizationInfo;
                graphicsPipelineInfo.pMultisampleState    =   &multisamplingInfo;
                graphicsPipelineInfo.pDynamicState        =   &dynamicStatesInfo;
                graphicsPipelineInfo.pColorBlendState     =   &colorblendInfo;
                graphicsPipelineInfo.pDepthStencilState   =   &depthStencilInfo;
                graphicsPipelineInfo.layout               =   pipelineLayout;
                graphicsPipelineInfo.renderPass           =   renderpass;
                graphicsPipelineInfo.subpass              =   subpass;
                graphicsPipelineInfo.basePipelineHandle   =   VK_NULL_HANDLE;
                graphicsPipelineInfo.pNext                =   nullptr;
            }

            GraphicsPipeline Build(VkRenderPass& renderpass,uint32_t subpass, Context& context)
            {
                if (pipelineLayout!=nullptr)
                {
                    vkDestroyPipelineLayout(context.device,pipelineLayout,nullptr);
                    pipelineLayout = nullptr;
                }
                if (graphicsPipeline!=nullptr)
                {
                    vkDestroyPipeline(context.device,graphicsPipeline,nullptr);
                    graphicsPipeline = nullptr;
                }

                Builder(renderpass,subpass,context);

                if (vkCreateGraphicsPipelines(context.device,nullptr,1,&graphicsPipelineInfo,nullptr,&graphicsPipeline) != VK_SUCCESS)
                    throw std::runtime_error("\nCan't create graphics pipeline!");

                GraphicsPipeline result = GraphicsPipeline(context,graphicsPipeline,pipelineLayout);
                graphicsPipeline = nullptr;
                pipelineLayout = nullptr;
                ResetGraphicsPipeline();

                return result;
            }

#pragma endregion

#pragma region Default values

    void ResetGraphicsPipeline()
    {
        ResetInputAssembly();
        ResetVertexInput();  // can be used for post-processing/lighting phase where full screen quad will be generated on the vertex shader
        ResetViewport();
        ResetDynamicStates();
        ResetShaderStages();
        ResetColorBlend();
        ResetRasterization();
        ResetMultisampling(); // default is off
        ResetDepthStencilState(); // depth for opaque, stecils - off
        ResetPipelineLayout();
    }

//                                          Constructor
    GraphicsPipelineBuilder()
    {
        ResetGraphicsPipeline();
    }

#pragma endregion

};
