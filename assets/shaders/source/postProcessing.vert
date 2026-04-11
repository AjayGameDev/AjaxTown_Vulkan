#version 450

layout(location = 0) out vec2 uv;


void main()
{
    // Draw one big triangle instead of a quad with two triangles for better cache efficiency
    // Read: https://michaldrobot.com/2014/04/01/gcn-execution-patterns-in-full-screen-passes/ and https://wallisc.github.io/rendering/2021/04/18/Fullscreen-Pass.html

    uv = vec2( (gl_VertexIndex<<1) & 2, gl_VertexIndex & 2 ); // uv = [0,2] intentionally for interpolation to work correctly, in fragment shader do uv = uv * .5f to get [0,1] range

    gl_Position = vec4(uv * 2 - 1,0.0,1.0); // Triangle (-1,-1) bottom left edge (3,-1) bottom right edge(offscreen) (-1,3) top edge(offscreen)

}