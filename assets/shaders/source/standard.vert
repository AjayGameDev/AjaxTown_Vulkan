#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_ARB_shader_draw_parameters : require

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 UV;
layout(location=3) in vec4 tangent;

layout(location=0) out vec2 uv;
layout(location=1) out flat int materialIndex;

layout(buffer_reference,std430,row_major) readonly buffer ModelMatricesBuffer { mat4 modelMatrices[]; };

layout(push_constant,row_major) uniform PushConstant
{
    mat4     viewProjectionMatrix;
    uint64_t modelMatricesAddress;
    uint64_t materialsAddress;
}pc;


void main()
{

    ModelMatricesBuffer modelMatricesBuffer = ModelMatricesBuffer(pc.modelMatricesAddress);
    //mat4 model = modelMatricesBuffer.modelMatrices[gl_BaseInstanceARB];
    mat4 model = modelMatricesBuffer.modelMatrices[gl_InstanceIndex];

    gl_Position = vec4(position,1) * model * pc.viewProjectionMatrix;
    uv = vec2(UV.x,1.0f-UV.y); // flip y for vulkan
    //materialIndex = gl_BaseInstanceARB;
    materialIndex = gl_InstanceIndex;
}

