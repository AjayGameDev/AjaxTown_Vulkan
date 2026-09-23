#include "common.slangh"
#include "platform.slangh"

struct VertexInput
{
    vec3 position : POSITION;
    vec3 normal : NORMAL;
    vec2 uv : TEXCOORD0;
    vec4 tangent : TANGENT;
};

struct VertexOutput
{
    vec4 position : SV_Position;
    vec3 normal : NORMAL;
    vec2 uv : TEXCOORD0;
    vec4 tangent : TANGENT;
    nointerpolation uint materialIndex : MATERIAL_INDEX;
};

struct Material
{
    uint albedoIndex;
    uint normalIndex;
    uint rmaoIndex;
    uint samplerIndex;
};

struct PushConstant
{
    mat4     viewProjectionMatrix;
    uint64_t modelMatricesAddress;
    uint64_t materialsAddress;
};

[[vk::binding(1, 0)]]
Texture2D textures[];

[[vk::binding(2, 0)]]
SamplerState samplers[];

[[vk::push_constant]]
PushConstant pc;





[shader("vertex")]
VertexOutput vertexMain(VertexInput input,uint localInstanceID : SV_InstanceID,uint baseInstance : SV_StartInstanceLocation)
{
    VertexOutput output;
    uint instanceID = baseInstance + localInstanceID; 

    mat4 *modelMatrices = (mat4*)pc.modelMatricesAddress;
    mat4 modelMatrix = modelMatrices[instanceID];    
 
    output.position = mul(mul(vec4(input.position, 1.0f), modelMatrix) , pc.viewProjectionMatrix);

    // here we are assuming that scale is uniform

    output.normal        =  mul(input.normal, mat3(modelMatrix));                            // calculate world space normal
    output.tangent       =  vec4(mul(input.tangent.xyz,mat3(modelMatrix)),input.tangent.w); // calculate world space tangent
    output.uv            =  vec2(input.uv.x, 1.0f - input.uv.y);                           // flip y for vulkan
    output.materialIndex =  instanceID;

    return output;
}


[shader("fragment")]
vec4 fragmentMain(VertexOutput input) : SV_Target
{
    // Interpolated normals from vertex to fragment shader lose their perpendicularity and unit length so always normalize them in fragment shader
    vec3 N = normalize(input.normal);
    vec3 T = normalize(input.tangent.xyz);
    vec3 B = cross(N, T) * input.tangent.w;

    mat3 TBN = mat3(T, B, N);


    Material *materials = (Material *)pc.materialsAddress;
    Material material = materials[input.materialIndex];

    vec4 diffuseColor, rmaoColor, normalColor, finalColor;

    // NonUniformResourceIndex is required so threads in the same group can access different index
    uint diffuseIndex = NonUniformResourceIndex(material.albedoIndex);
    uint normalIndex  = NonUniformResourceIndex(material.normalIndex);
    uint rmaoIndex    = NonUniformResourceIndex(material.rmaoIndex);
    uint samplerIndex = NonUniformResourceIndex(material.samplerIndex);

    diffuseColor = textures[diffuseIndex].Sample(samplers[samplerIndex], input.uv);
    normalColor  = textures[normalIndex].Sample(samplers[samplerIndex], input.uv);
    rmaoColor    = textures[rmaoIndex].Sample(samplers[samplerIndex], input.uv);

    vec3 normal = vec3(0.0f, 0.0f, 0.0f);

    #ifdef astc
        normal.xy = normalColor.ra; 
    #endif
    
    #ifdef bc
        normal.xy = normalColor.rg;
    #endif


    normal.xy = normal.xy * 2.0f - 1.0f; // unpack tangent space normal from [0,1] to [-1,1] range
    normal.z  = sqrt(max(0.0f, 1.0f - dot(normal.xy, normal.xy))); // calculate z component based on the fact that it has unit length

    normal = normalize(mul(normal, TBN)); // moving normal from tangent space to world space 


    // use this for displaying normal map info other wise it will be dark blue(which is correct but for calculations) 
    finalColor = vec4(normal.xyz * .5f + .5f,1.0f);

    return finalColor; 
}