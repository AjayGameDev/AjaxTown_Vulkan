#pragma once
#include "../../../../AppData/Local/Android/Sdk/ndk/29.0.13113456/sources/android/cpufeatures/Maths.h"
#include <cstdint>

struct Transform
{
	Vector3   position;
	Rotation  rotation;
	Vector3   scale;
	Matrix4   modelMatrix;

	Transform(float positionX, float positionY, float positionZ, float RotationX, float RotationY, float RotationZ, float ScaleX, float ScaleY, float ScaleZ)
	{
		this->position.x = positionX;
		this->position.y = positionY;
		this->position.z = positionZ;

		this->rotation.eulerAngles.x = RotationX;
		this->rotation.eulerAngles.y = RotationY;
		this->rotation.eulerAngles.z = RotationZ;

		

		this->scale.x = ScaleX;
		this->scale.y = ScaleY;
		this->scale.z = ScaleZ;

		GenerateIdentityMatrix(modelMatrix); 
		GenerateModelMatrix(modelMatrix, position, rotation, scale);

	}
};

