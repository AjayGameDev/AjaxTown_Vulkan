#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_shader_explicit_arithmetic_types : require

layout(location=0) out vec4 finalColor;
layout(location=0) in vec2 UV;
layout(location=1) in flat int materialIndex;

struct Material
{
    uint32_t albedoIndex;
    uint32_t normalIndex;
    uint32_t rmaoIndex;
    uint32_t samplerIndex;
};

layout(buffer_reference,std430) readonly buffer MaterialBuffer { Material materials[]; };

layout(push_constant,row_major) uniform PushConstant
{
    mat4     viewProjectionMatrix;
    uint64_t modelMatricesAddress;
    uint64_t materialsAddress;
}pc;

layout(set=0,binding=1) uniform texture2D textures[];
layout(set=0,binding=2) uniform sampler   samplers[];

void main()
{
    MaterialBuffer materialBuffer = MaterialBuffer(pc.materialsAddress);
    Material material = materialBuffer.materials[materialIndex];

    //finalColor = vec4(1.0f,.5f,.25f,1.0f);
    finalColor = texture(sampler2D( textures[nonuniformEXT(material.albedoIndex)],
                                    samplers[nonuniformEXT(material.samplerIndex)]),
                                    UV
                                   );
    //finalColor.a = 1.0f;
}