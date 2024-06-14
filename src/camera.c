#include "camera.h"

// Returns the cameras up vector (normalized)
// Note: The up vector might not be perpendicular to the forward vector
Vector3 _GetCameraUp(Camera* camera)
{
    return Vector3Normalize(camera->up);
}

// Returns the cameras forward vector (normalized)
Vector3 _GetCameraForward(Camera* camera)
{
    return Vector3Normalize(Vector3Subtract(camera->target, camera->position));
}

// Returns the cameras right vector (normalized)
Vector3 _GetCameraRight(Camera* camera)
{
    Vector3 forward = _GetCameraForward(camera);
    Vector3 up = _GetCameraUp(camera);

    return Vector3CrossProduct(forward, up);
}

// Rotates the camera around its up vector
// Yaw is "looking left and right"
// If rotateAroundTarget is false, the camera rotates around its position
// Note: angle must be provided in radians
void _CameraYaw(Camera* camera, float angle, bool rotateAroundTarget)
{
    // Rotation axis
    Vector3 up = _GetCameraUp(camera);

    // View vector
    Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

    // Rotate view vector around up axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, up, angle);

    if (rotateAroundTarget) {
        // Move position relative to target
        camera->position = Vector3Subtract(camera->target, targetPosition);
    } else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = Vector3Add(camera->position, targetPosition);
    }
}

// Rotates the camera around its right vector, pitch is "looking up and down"
//  - lockView prevents camera overrotation (aka "somersaults")
//  - rotateAroundTarget defines if rotation is around target or around its position
//  - rotateUp rotates the up direction as well (typically only usefull in CAMERA_FREE)
// NOTE: angle must be provided in radians
void _CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    // Up direction
    Vector3 up = _GetCameraUp(camera);

    // View vector
    Vector3 targetPosition = Vector3Subtract(camera->target, camera->position);

    if (lockView) {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // Clamp view up
        float maxAngleUp = Vector3Angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (angle > maxAngleUp)
            angle = maxAngleUp;

        // Clamp view down
        float maxAngleDown = Vector3Angle(Vector3Negate(up), targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (angle < maxAngleDown)
            angle = maxAngleDown;
    }

    // Rotation axis
    Vector3 right = _GetCameraRight(camera);

    // Rotate view vector around right axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, right, angle);

    if (rotateAroundTarget) {
        // Move position relative to target
        camera->position = Vector3Subtract(camera->target, targetPosition);
    } else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = Vector3Add(camera->position, targetPosition);
    }

    if (rotateUp) {
        // Rotate up direction around right axis
        camera->up = Vector3RotateByAxisAngle(camera->up, right, angle);
    }
}

// Moves the camera position closer/farther to/from the camera target
void _CameraMoveToTarget(Camera* camera, float delta)
{
    float distance = Vector3Distance(camera->position, camera->target);

    // Apply delta
    distance += delta;

    // Distance must be greater than 0
    if (distance <= 0)
        distance = 0.001f;

    // Set new distance by moving the position along the forward vector
    Vector3 forward = _GetCameraForward(camera);
    camera->position = Vector3Add(camera->target, Vector3Scale(forward, -distance));
}
