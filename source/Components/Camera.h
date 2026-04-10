#pragma once
#include <Maths.h>
#include <cmath>
#include "Transform.h"

struct Camera
{
    float verticalFov;  // vertical fov in radians
    float aspectRatio; // width / height 16:9 or 21:9 or 1 for square
    float nearPlane;  // .1f or .01f (too small value might introduce z fighting)
    Matrix4 projectionMatrix; // we will use reverse z with much better precision by setting far to infinite
    Matrix4 viewMatrix;
    Matrix4 viewProjectionMatrix;
    bool isDirty;

    Camera()
    {
        Update(60.0f,16.0/9.0f,.01f); // don't use 16/9 for aspect ratio as it will be seen as integer divide and will give zero
    }

    // fov = vertical fov in degrees | aspectRatio = width / height 16:9 | nearPlane = .1f or .01f
    Camera(float fov,float aspectRatio,float nearPlane)
    {
        Update(fov,aspectRatio,nearPlane);
    }

    void Update(float fov,float aspectRatio,float nearPlane)
    {
        this->verticalFov = fov * degreeToRadians; // degree to radians
        this->aspectRatio = aspectRatio;
        this->nearPlane   = nearPlane;
        this->isDirty     = true;

        GenerateProjectionMatrix();
    }

    void GenerateProjectionMatrix()
    {
        if (!isDirty)
            return;

        isDirty = false;

        float f = 1.0f  / tanf(verticalFov * .5f);

        projectionMatrix[0][0] = f/aspectRatio;
        projectionMatrix[0][1] = 0;
        projectionMatrix[0][2] = 0;
        projectionMatrix[0][3] = 0;

        projectionMatrix[1][0] = 0;
        projectionMatrix[1][1] = -f; // might need to flip to -f or use negative height in viewport.height = -swapchainHeight
        projectionMatrix[1][2] = 0;
        projectionMatrix[1][3] = 0;

        projectionMatrix[2][0] = 0;
        projectionMatrix[2][1] = 0;
        projectionMatrix[2][2] = 0;
        projectionMatrix[2][3] = 1;

        projectionMatrix[3][0] = 0;
        projectionMatrix[3][1] = 0;
        projectionMatrix[3][2] = nearPlane;
        projectionMatrix[3][3] = 0;
    }

    void GenerateViewMatrix(Transform& transform)
    {
        float x2 = transform.rotation.x * transform.rotation.x, y2 = transform.rotation.y * transform.rotation.y, z2 = transform.rotation.z * transform.rotation.z;
        float xy = transform.rotation.x * transform.rotation.y, xz = transform.rotation.x * transform.rotation.z,wx = transform.rotation.w * transform.rotation.x, wz = transform.rotation.w * transform.rotation.z, wy = transform.rotation.w * transform.rotation.y, yz = transform.rotation.y * transform.rotation.z;

        float Rx = 1 - 2 * (y2+z2), Ry = 2 * (xy+wz),     Rz = 2 * (xz - wy);
        float Ux = 2 * (xy-wz),     Uy = 1 - 2 * (x2+z2), Uz = 2 * (yz + wx);
        float Fx = 2 * (xz+wy),     Fy = 2 * (yz-wx),     Fz = 1 - 2 * (x2 + y2);
        float Px = transform.positionScale.x, Py = transform.positionScale.y, Pz = transform.positionScale.z;

        viewMatrix[0][0] = Rx;
        viewMatrix[0][1] = Ux;
        viewMatrix[0][2] = Fx;
        viewMatrix[0][3] = 0;

        viewMatrix[1][0] = Ry;
        viewMatrix[1][1] = Uy;
        viewMatrix[1][2] = Fy;
        viewMatrix[1][3] = 0;

        viewMatrix[2][0] = Rz;
        viewMatrix[2][1] = Uz;
        viewMatrix[2][2] = Fz;
        viewMatrix[2][3] = 0;

        viewMatrix[3][0] = -(Rx * Px + Ry * Py + Rz * Pz);
        viewMatrix[3][1] = -(Ux * Px + Uy * Py + Uz * Pz);
        viewMatrix[3][2] = -(Fx * Px + Fy * Py + Fz * Pz);
        viewMatrix[3][3] = 1;


    }

    void GenerateViewProjectionMatrix(Transform& transform)
    {
        GenerateViewMatrix(transform);

        // V * P so in shader do vec4 gl_position = pos * M * V * P
        viewProjectionMatrix[0][0] = viewMatrix[0][0] * projectionMatrix[0][0];
        viewProjectionMatrix[0][1] = viewMatrix[0][1] * projectionMatrix[1][1];
        viewProjectionMatrix[0][2] = 0;
        viewProjectionMatrix[0][3] = viewMatrix[0][2];

        viewProjectionMatrix[1][0] = viewMatrix[1][0] * projectionMatrix[0][0];
        viewProjectionMatrix[1][1] = viewMatrix[1][1] * projectionMatrix[1][1];
        viewProjectionMatrix[1][2] = 0;
        viewProjectionMatrix[1][3] = viewMatrix[1][2];

        viewProjectionMatrix[2][0] = viewMatrix[2][0] * projectionMatrix[0][0];
        viewProjectionMatrix[2][1] = viewMatrix[2][1] * projectionMatrix[1][1];
        viewProjectionMatrix[2][2] = 0;
        viewProjectionMatrix[2][3] = viewMatrix[2][2];

        viewProjectionMatrix[3][0] = viewMatrix[3][0] * projectionMatrix[0][0];
        viewProjectionMatrix[3][1] = viewMatrix[3][1] * projectionMatrix[1][1];
        viewProjectionMatrix[3][2] = projectionMatrix[3][2];
        viewProjectionMatrix[3][3] = viewMatrix[3][2];
    }
};