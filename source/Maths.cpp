#include "Maths.h"

void GenerateIdentityMatrix(Matrix4 matrix)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
                matrix[i][j] = 1;
            else
                matrix[i][j] = 0;
        }
    }
}


// converts angle in degree to radians as trignometric functions use radians
void DegreeToRadians(const float & angleInDegrees, float& angleInRadians)
{
    angleInRadians = angleInDegrees * 0.017453f;
}
void Swap(float& a, float& b)
{
    float c;
    c = a;
    a = b;
    b = c;
}

void TransposeMatrix(Matrix4 matrix) 
{
    for (int i = 0; i < 4; ++i) 
    {
        for (int j = i + 1; j < 4; ++j) 
        {
            Swap(matrix[i][j], matrix[j][i]);
        }
    }
}

void VerticalFov(float& fovV, const float & fovH, const float & aspectRatio) // calculating vertical fov from horizontal fov
{
    fovV = 2 * atan((1 / aspectRatio) * tan(fovH / 2));
}

void GenerateProjectionMatrix(Matrix4 matrix, float fovV, const float aspectRatio, const float near, const float far)
{

    fovV *= degreeToRadians;
    //VerticalFov(fovV, fovH, aspectRatio);

    matrix[0][0] = 1 / (aspectRatio * tan(fovV / 2));
    matrix[1][1] = 1 / tan(fovV / 2);
    matrix[2][2] = (far + near) / (near - far);
    matrix[3][3] = 0;
    matrix[2][3] = -1;
    matrix[3][2] = (2 * far * near) / (near - far);

}

void EulerAnglesToQuaternion(EulerAngles euler, Quaternion& quaternion)
{

    // -------------------------------------------- Converting degree to radians
    euler.x *= degreeToRadians;
    euler.y *= degreeToRadians;
    euler.z *= degreeToRadians;

    float cx = cos(euler.x / 2);
    float cy = cos(euler.y / 2);
    float cz = cos(euler.z / 2);

    float sx = sin(euler.x / 2);
    float sy = sin(euler.y / 2);
    float sz = sin(euler.z / 2);

    quaternion.w = cx * cy * cz + sx * sy * sz;
    quaternion.x = sx * cy * cz - cx * sy * sz;
    quaternion.y = cx * sy * cz + sx * cy * sz;
    quaternion.z = cx * cy * sz - sx * sy * cz;

}


void GenerateViewMatrix(Matrix4 matrix,float pitch,float yaw,float roll,float px,float py,float pz)
{
    // -------------------------------------------- Converting degree to radians
    pitch *= degreeToRadians;
    yaw   *= degreeToRadians;
    roll  *= degreeToRadians;

    pz *= -1; // to maked forward as +ve z and backsward -ve

    // -------------------------------------------- Calculating quternion from euler angles

    float w, x, y, z;

    float cx = cos(pitch / 2);
    float cy = cos(yaw / 2);
    float cz = cos(roll / 2);
    
    float sx = sin(pitch / 2);
    float sy = sin(yaw / 2);
    float sz = sin(roll / 2);

    // This is for  pitch yaw roll sequence

    //w = cx * cy * cz   +   sx * sy * sz;
    //x = sx * cy * cz   -   cx * sy * sz;
    //y = cx * sy * cz   +   sx * cy * sz;
    //z = cx * cy * sz   -   sx * sy * cz;   



    // This is for yaw pitch roll 
    //w = cx * cy * cz + sx * sy * sz;
    //x = sx * cy * cz - cx * sy * sz;
    //y = cx * sy * cz + sx * cy * sz;
    //z = cx * cy * sz - sx * sy * cz;

    // This is for roll pitch yaw sequence
    w = cx * cy * cz - sx * sy * sz;
    x = sx * cy * cz + cx * sy * sz;
    y = cx * sy * cz - sx * cy * sz;
    z = cx * cy * sz + sx * sy * cz;

    float magnitude = SqrtFast(w * w + x * x + y * y + z * z);
    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;

    // -------------------------------------------- Calculating view matrix rotation part

    matrix[0][0] = 1 - 2 * (y*y + z*z);
    matrix[0][1] = 2 * (x*y - w*z);
    matrix[0][2] = 2 * (x*z + w*y);
    //matrix[0][3] = 0;

    matrix[1][0] = 2 * (x*y + w*z);
    matrix[1][1] = 1 - 2 * (x*x + z*z);
    matrix[1][2] = 2 * (y*z - w*x);
    //matrix[1][3] = 0;

    matrix[2][0] = 2 * (x*z - w*y);
    matrix[2][1] = 2 * (y*z + w*x);
    matrix[2][2] = 1 - 2 * (x*x + y*y);
    //matrix[2][3] = 0;

    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;

    // -------------------------------------------- Calculating view matrix translation part

    matrix[0][3] = -(matrix[0][0] * px + matrix[0][1] * py + matrix[0][2] * pz);
    matrix[1][3] = -(matrix[1][0] * px + matrix[1][1] * py + matrix[1][2] * pz);
    matrix[2][3] = -(matrix[2][0] * px + matrix[2][1] * py + matrix[2][2] * pz);

    // Opengl assumes column major matrix because of internal math conventions which is opposite of how we interpret matrices(row major)
    // By transposing the matrix we will get the desired result
    TransposeMatrix(matrix); 
}


// -------------------------------------------- Calculating model matrix

void GenerateModelMatrix(Matrix4 matrix, Vector3 position, Rotation rotation, Vector3 scale)
{
    EulerAnglesToQuaternion(rotation.eulerAngles, rotation.quaternion);

    matrix[0][0] = (1 - 2 * (rotation.quaternion.y * rotation.quaternion.y + rotation.quaternion.z * rotation.quaternion.z))  * scale.x;
    matrix[0][1] = (2 * (rotation.quaternion.x * rotation.quaternion.y - rotation.quaternion.w * rotation.quaternion.z))      * scale.x;
    matrix[0][2] = (2 * (rotation.quaternion.x * rotation.quaternion.z + rotation.quaternion.w * rotation.quaternion.y))      * scale.x;
    matrix[0][3] = position.x; // Translation x

    matrix[1][0] = (2 * (rotation.quaternion.x * rotation.quaternion.y + rotation.quaternion.w * rotation.quaternion.z))     * scale.y;
    matrix[1][1] = (1 - 2 * (rotation.quaternion.x * rotation.quaternion.x + rotation.quaternion.z * rotation.quaternion.z)) * scale.y;
    matrix[1][2] = (2 * (rotation.quaternion.y * rotation.quaternion.z - rotation.quaternion.w * rotation.quaternion.x))     * scale.y;
    matrix[1][3] = position.y; // Translation y

    matrix[2][0] = (2 * (rotation.quaternion.x * rotation.quaternion.z - rotation.quaternion.w * rotation.quaternion.y))     * scale.z;
    matrix[2][1] = (2 * (rotation.quaternion.y * rotation.quaternion.z + rotation.quaternion.w * rotation.quaternion.x))     * scale.z;
    matrix[2][2] = (1 - 2 * (rotation.quaternion.x * rotation.quaternion.x + rotation.quaternion.y * rotation.quaternion.y)) * scale.z;
    matrix[2][3] = position.z; // Translation z

    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;


}


void Extract3x3Matrix(Matrix4& source, Matrix4& target)
{
    // Extracting the top-left 3x3 part of the 4x4 view matrix

    target[0][0] = source[0][0];
    target[0][1] = source[0][1];
    target[0][2] = source[0][2];
                           
    target[1][0] = source[1][0];
    target[1][1] = source[1][1];
    target[1][2] = source[1][2];
                           
    target[2][0] = source[2][0];
    target[2][1] = source[2][1];
    target[2][2] = source[2][2];

    // The last row and column of the target matrix should remain unchanged
    target[3][0] = 0;
    target[3][1] = 0;
    target[3][2] = 0;
    target[3][3] = 1;
}



