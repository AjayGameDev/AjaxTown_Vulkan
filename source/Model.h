#pragma once

struct aiScene; // forward declaration, don't use class here otherwise assimp will give errors


struct Vertex_Standard
{
  Vector3 position;
  Vector3 normal;
  Vector2 uv;
  Vector4 tangent; // w = ±1 for handedness of bitangent
  //Vector3 bitangent; // we can derive it like Vector3 bitangent = Vector3.Cross(normal,tangent.xyz) * tangent.w

};

struct Vertex_Minimal
{
  Vector3 position;
  Vector2 uv;
};

struct Vertex_Skinned
{
  Vector3 position;
  Vector3 normal;
  Vector2 uv;
  Vector4 tangent;
  uint4   boneIndices;  // uint 4, each vertex can be affected by max 4 bones
  Vector4 boneWeights; //  should sum upto 1(normalized) or can be normalized in the shader
};

class Model
{
  private:
          std::vector<Vertex_Standard> vertices;
          std::vector<uint32_t> indices;
  public:
          explicit Model(const std::string& modelName);
          ~Model();
          void Process(const aiScene* scene);
          void LoadModel(const std::string& modelName);
          std::vector<Vertex_Standard>& GetVertices() {return vertices;}
          std::vector<uint32_t>& GetIndices() {return indices;}
};
