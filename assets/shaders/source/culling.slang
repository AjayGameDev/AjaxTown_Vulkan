#include "common.slangh"

struct DrawCommand
{
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    int vertexOffset;
    uint firstInstance; 
};

struct Transform
{
    vec4 posScale;
    vec4 rotation;
};

struct Mesh
{
    uint indexCount;
    uint instanceCount;
    uint indexOffset;
    int vertexOffset;
    uint firstInstance;
    uint materialIndex;
};

struct PushConstant
{
    uint64_t transformsAddress;
    uint64_t modelMatricesAddress;
    uint64_t meshesAddress;
    uint meshCount;
};

[[vk::binding(0,0)]] //(binding,set)
RWStructuredBuffer<DrawCommand> drawCommands;

[[vk::binding(1, 0)]]
RWStructuredBuffer<uint> drawCount;

[[vk::push_constant]]
PushConstant pc;






[shader("compute")]
[numthreads(64,1,1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    uint id = dispatchThreadID.x;

    if (id >= pc.meshCount)
        return;

    Transform* transforms = (Transform*)pc.transformsAddress; // Read-only buffer
    Mesh* meshes = (Mesh*)pc.meshesAddress; // Read-only

    mat4* modelMatrices = (mat4*)pc.modelMatricesAddress; // Row major by default,Read and write

    Transform transform = transforms[id];
    vec4 posScale = transform.posScale;
    vec4 rotation = transform.rotation; 

    mat4 modelMatrix;

    float x2 = rotation.x * rotation.x, y2 = rotation.y * rotation.y, z2 = rotation.z * rotation.z;
    float xy = rotation.x * rotation.y, xz = rotation.x * rotation.z, wx = rotation.w * rotation.x, wz = rotation.w * rotation.z, wy = rotation.w * rotation.y, yz = rotation.y * rotation.z;
    float s = posScale.w;

    

    //glsl uses A[column][row] but slang uses A[row][column]
    modelMatrix[0][0] = s * (1 - 2 * (y2 + z2));
    modelMatrix[1][0] = s * (2 * (xy + wz));
    modelMatrix[2][0] = s * (2 * (xz - wy));
    modelMatrix[3][0] = posScale.x;

    modelMatrix[0][1] = s * (2 * (xy - wz));
    modelMatrix[1][1] = s * (1 - 2 * (x2 + z2));
    modelMatrix[2][1] = s * (2 * (yz + wx));
    modelMatrix[3][1] = posScale.y;

    modelMatrix[0][2] = s * (2 * (xz + wy));
    modelMatrix[1][2] = s * (2 * (yz - wx));
    modelMatrix[2][2] = s * (1 - 2 * (x2 + y2));
    modelMatrix[3][2] = posScale.z;

    modelMatrix[0][3] = 0;
    modelMatrix[1][3] = 0;
    modelMatrix[2][3] = 0;
    modelMatrix[3][3] = 1;



    modelMatrices[id] = modelMatrix;

    Mesh mesh = meshes[id];

    drawCommands[id].indexCount = mesh.indexCount;
    drawCommands[id].firstIndex = mesh.indexOffset;
    drawCommands[id].vertexOffset = mesh.vertexOffset;
    drawCommands[id].instanceCount = mesh.instanceCount;
    // drawCommands[id].firstInstance  =  mesh.firstInstance;
    drawCommands[id].firstInstance = mesh.materialIndex;

    InterlockedAdd(drawCount[0], 1);

}
