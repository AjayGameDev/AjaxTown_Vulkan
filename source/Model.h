#pragma once

#include "Maths.h"

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
  uint4   boneIndices;  // uInt 4, each vertex can be affected by max 4 bones
  Vector4 boneWeights; //  should sum upto 1(normalized) or can be normalized in the shader
};