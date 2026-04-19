#pragma once
#include "Maths.h"

// Instead of sending model matrix with 4x4 = 16 floats (64 byes)
// we can use quaternion float 4 + posScale float 4 = 8 floats (32 bytes)
// that's saving 50% bandwidth and better cache hits [for more details read this https://tech.metail.com/performance-quaternions-gpu/ or search in performance section of screenshots in graphics programming folder]

struct Transform
{
	Vector4	   positionScale;
	Quaternion rotation;
	//Vector3   position;
	//Rotation  rotation;
	//Vector3   scale;
	//Matrix4   modelMatrix;

	//Transform()
	//{
	//	positionScale.x = 0;
	//	positionScale.y = 0;
	//	positionScale.z = 0;
	//	positionScale.w = 1; // by default scale is 1
//
	//	rotation.x = 0;
	//	rotation.y = 0;
	//	rotation.z = 0;
	//	rotation.w = 1;
//
	//}

	explicit Transform(float positionX = 0.0f, float positionY = 0.0f, float positionZ = 0.0f,float scale = 1.0f, float RotationX = 0.0f, float RotationY = 0.0f, float RotationZ = 0.0f,float RotationW = 1.0f)
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

	void SetRotationEuler(float yaw,float pitch,float roll)
	{

		float hy =  yaw    * 0.5f * degreeToRadians;
		float hp = -pitch  * 0.5f * degreeToRadians;  // negated for +Z forward
		float hr =  roll   * 0.5f * degreeToRadians;

		float sy = sinf(hy), cy = cosf(hy);
		float sp = sinf(hp), cp = cosf(hp);
		float sr = sinf(hr), cr = cosf(hr);

		rotation.x = cy*sp*cr + sy*cp*sr;
		rotation.y = sy*cp*cr - cy*sp*sr;
		rotation.z = cy*cp*sr - sy*sp*cr;
		rotation.w = cy*cp*cr + sy*sp*sr;
	}

	void SetPosition(float x,float y,float z)
	{
		positionScale.x = x;
		positionScale.y = y;
		positionScale.z = z;
	}

	void SetScale(float scale)
	{
		positionScale.w = scale;
	}
	/*
	void GenerateModelMatrix()
	{
		float x2 = rotation.x * rotation.x, y2 = rotation.y * rotation.y, z2 = rotation.z * rotation.z;
		float xy = rotation.x * rotation.y, xz = rotation.x * rotation.z,wx = rotation.w * rotation.x, wz = rotation.w * rotation.z, wy = rotation.w * rotation.y, yz = rotation.y * rotation.z;
		float s  = positionScale.w;

		modelMatrix[0][0] = s * (1 - 2 * (y2+z2));
		modelMatrix[0][1] = s * (2 * (xy+wz));
		modelMatrix[0][2] = s * (2 * (xz-wy));
		modelMatrix[0][3] = 0;

		modelMatrix[1][0] = s * (2 * (xy-wz));
		modelMatrix[1][1] = s * (1 - 2 * (x2+z2));
		modelMatrix[1][2] = s * (2 * (yz+wx));
		modelMatrix[1][3] = 0;

		modelMatrix[2][0] = s * (2 * (xz+wy));
		modelMatrix[2][1] = s * (2 * (yz-wx));
		modelMatrix[2][2] = s * (1 - 2 * (x2+y2));
		modelMatrix[2][3] = 0;

		modelMatrix[3][0] = positionScale.x;
		modelMatrix[3][1] = positionScale.y;
		modelMatrix[3][2] = positionScale.z;
		modelMatrix[3][3] = 1;

	}

	*/
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

