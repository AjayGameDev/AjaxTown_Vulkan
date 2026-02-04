#pragma once


#define degreeToRadians 0.017453f





//-----------------------------------------------------------------------  Square Root Calculation

inline float SqrtFast(float value)
{
	float half = 0.5f * value;
	int i = *(int*)&value;          // Floating-point to integer bit-cast
	i = 0x5f3759df - (i >> 1);      // Magic number and bit manipulation
	value = *(float*)&i;            // Integer back to float
	value = value * (1.5f - (half * value * value));  // Newton's method for refinement
	return value;            // Return the square root
}

inline float InverseSqrtFast(float value)
{
	float half = 0.5f * value;
	int i = *(int*)&value;          // Floating-point to integer bit-cast
	i = 0x5f3759df - (i >> 1);      // Magic number and bit manipulation
	value = *(float*)&i;            // Integer back to float
	value = value * (1.5f - (half * value * value));  // Newton's method for refinement
	return 1.0f / value;            // Return the inverse square root
}

//-----------------------------------------------------------------------  Vector 3

struct Vector3
{
	float x, y, z;

	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3(float value)
	{
		x = value;
		y = value;
		z = value;
	}

	Vector3(float a,float b,float c)
	{
		x = a;
		y = b;
		z = c;
	}

	void Normalize()
	{
		float inverseMagnitude;
		inverseMagnitude = InverseSqrtFast(x * x + y * y + z * z);

		if (inverseMagnitude > 0)
		{
			x *= inverseMagnitude;
			y *= inverseMagnitude;
			z *= inverseMagnitude;
		}

	}

	static Vector3 Cross(const Vector3& a, const Vector3& b)
	{
		return Vector3
		(
			a.y * b.z - a.z * b.y,  // x-component
			a.z * b.x - a.x * b.z,  // y-component
			a.x * b.y - a.y * b.x   // z-component
		);
	}
	
	static float Dot(const Vector3& a, const Vector3& b)
	{
		return 
				a.x * b.x + 
				a.y * b.y + 
				a.z * b.z;
	}
	// ---------------------------------------------  Operator Overloading  --------------------------------------------------
	Vector3 operator+(const Vector3& other)
	{
		return Vector3
		(
			x + other.x,
			y + other.y,
			z + other.z
		);
	}

	Vector3 operator-(const Vector3& other)
	{
		return Vector3
		(
			x - other.x,
			y - other.y,
			z - other.z
		);
	}

	Vector3 operator*(const Vector3& other)
	{
		return Vector3
		(
			x * other.x,
			y * other.y,
			z * other.z
		);
	}

	Vector3 operator/(const Vector3& other)
	{
		return Vector3
		(
			x / other.x,
			y / other.y,
			z / other.z
		);
	}

};

//-----------------------------------------------------------------------  Vector 4

struct Vector4
{
	float x, y,z,w;
	Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Vector4(float value)
	{
		x = value;
		y = value;
		z = value;
		w = value;
	}

	Vector4(float a, float b,float c, float d)
	{
		x = a;
		y = b;
		z = c;
		w = d;
	}
};


//-----------------------------------------------------------------------  uint 4

struct uint4
{
	unsigned int x, y,z,w;

	uint4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	uint4(unsigned int value)
	{
		x = value;
		y = value;
		z = value;
		w = value;
	}

	uint4(unsigned int a, unsigned int b,unsigned int c, unsigned int d)
	{
		x = a;
		y = b;
		z = c;
		w = d;
	}
};



//-----------------------------------------------------------------------  Vector 2




struct Vector2
{
	float x, y;
	Vector2()
	{
		x = 0;
		y = 0;
	}
	Vector2(float value)
	{
		x = value;
		y = value;
	}
	Vector2(float a, float b)
	{
		x = a;
		y = b;
	}
};

//-----------------------------------------------------------------------  Euler and quaternion

struct EulerAngles
{
	float x, y, z;
	EulerAngles()
	{
		x = 0;
		y = 0;
		z = 0;
	}
};
struct Quaternion
{
	float x, y, z, w; 
	Quaternion()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}
};

struct Rotation
{
	EulerAngles eulerAngles;
	Quaternion quaternion;
};

//-----------------------------------------------------------------------  Helper Functions

typedef float Matrix4[4][4];

void GenerateIdentityMatrix(Matrix4& matrix);

void GenerateProjectionMatrix(Matrix4& matrix,float fovH,float aspectRatio,float near,float far);

void GenerateViewMatrix(Matrix4& matrix, float pitch, float yaw, float roll, float px, float py, float pz);

void GenerateModelMatrix(Matrix4& matrix, Vector3 position, Rotation rotation, Vector3 scale);

void Extract3x3Matrix(Matrix4& source, Matrix4& target);



