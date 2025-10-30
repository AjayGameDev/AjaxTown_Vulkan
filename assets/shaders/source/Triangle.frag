#version 450

layout(location = 0) in  vec3 fragColor;

layout(location=0) out vec4 outAlbedo;
layout(location=1) out vec4 outNormal;
layout(location=2) out vec4 outRMAO;
layout(location=3) out vec4 color;

void main()
{
    outAlbedo = vec4(fragColor,1.0);
    outNormal = vec4(0.5,0.5,1.0,1.0);
    outRMAO   = vec4(0.0,0.0,0.0,1.0);
    color = vec4(fragColor,1.0);
}