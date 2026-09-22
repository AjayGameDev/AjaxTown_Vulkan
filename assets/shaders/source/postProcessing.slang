#include "common.slangh"


struct VertexToFragment
{
    vec2 uv : TEXCOORD0;
    vec4 position : SV_Position;
};

[[vk::input_attachment_index(0)]]
[[vk::binding(0,0)]] // (binding,set)
SubpassInput hdrImage;


[shader("vertex")]
VertexToFragment vertexMain(uint vertexID : SV_VertexID)
{
    VertexToFragment output;


    // Draw one big triangle instead of a quad with two triangles for better cache efficiency
    // Read: https://michaldrobot.com/2014/04/01/gcn-execution-patterns-in-full-screen-passes/ and https://wallisc.github.io/rendering/2021/04/18/Fullscreen-Pass.html

    output.uv = vec2((vertexID << 1) & 2, vertexID & 2); // uv = [0,2] intentionally for interpolation to work correctly, in fragment shader do uv = uv * .5f to get [0,1] range

    output.position = vec4(output.uv * 2 - 1, 0.0, 1.0); // Triangle (-1,-1) bottom left edge (3,-1) bottom right edge(offscreen) (-1,3) top edge(offscreen)

    return output;
}


[shader("fragment")]
vec4 fragmentMain(VertexToFragment input) : SV_Target
{
    vec2 uv = input.uv * 0.5f; // to go back from [0,2] to [0,1]
    vec4 hdrColor = hdrImage.SubpassLoad();
    vec4 finalColor = vec4(hdrColor.rgb, 1.0f);
    return finalColor;          
}