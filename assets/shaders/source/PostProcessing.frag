#version 450

layout(location = 0) in vec2 uvInput;

layout(location = 0) out vec4 finalColor;

layout(input_attachment_index = 0,set = 0,binding = 1) uniform subpassInput hdrImage;

void main()
{
    vec2 uv    = uvInput * 0.5f;        // to go back from [0,2] to [0,1]
    finalColor = subpassLoad(hdrImage);
    if(finalColor.x == 0 && finalColor.y == 0)
        finalColor = vec4(0,0,0,0);
}