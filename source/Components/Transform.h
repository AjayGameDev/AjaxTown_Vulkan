#pragma once
#include "Maths.h"

struct Transform
{
	Vector4	   positionScale;
	Quaternion rotation;
	//Vector3   position;
	//Rotation  rotation;
	//Vector3   scale;
	//Matrix4   modelMatrix;

	explicit Transform(float positionX = 0.0f, float positionY = 0.0f, float positionZ = 0.0f, float RotationX = 0.0f, float RotationY, float RotationZ = 0.0f,float RotationW = 1.0f, float scale = 1.0f)
	{
		this->positionScale.x = positionX;
		this->positionScale.y = positionY;
		this->positionScale.z = positionZ;

		this->positionScale.w = scale;

		this->rotation.x = RotationX;
		this->rotation.y = RotationY;
		this->rotation.z = RotationZ;
		this->rotation.w = RotationW;

	}
	/*
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
	*/
};

