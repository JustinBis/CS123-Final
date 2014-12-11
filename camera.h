#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"

class Camera
{
public:
    // Initialize your camera.
    Camera();

    // Sets the aspect ratio of this camera. Automatically called by the GUI when the window is
    // resized.
    void setAspectRatio(float aspectRatio);

    // Returns the projection matrix given the current camera settings.
    glm::mat4x4 getProjectionMatrix() const;

    // Returns the view matrix given the current camera settings.
    glm::mat4x4 getViewMatrix() const;

    // Returns the matrix that scales down the perspective view volume into the canonical
    // perspective view volume, given the current camera settings.
    glm::mat4x4 getScaleMatrix() const;

    // Returns the matrix the unhinges the perspective view volume, given the current camera
    // settings.
    glm::mat4x4 getPerspectiveMatrix() const;

    // Returns the current position of the camera.
    glm::vec4 getPosition() const;

    // Returns the current 'look' vector for this camera.
    glm::vec4 getLook() const;

    // Returns the current 'up' vector for this camera (the 'V' vector).
    glm::vec4 getUp() const;

    // Returns the currently set aspect ratio.
    float getAspectRatio() const;

    // Returns the currently set height angle.
    float getHeightAngle() const;

    // Move this camera to a new eye position, and orient the camera's axes given look and up
    // vectors.
    void orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up);

    // Sets the height angle of this camera.
    void setHeightAngle(float h);

    // Translates the camera along a given vector.
    void translate(const glm::vec4 &v);

    // Rotates the camera about the U axis by a specified number of degrees.
    void rotateU(float degrees);

    // Rotates the camera about the V axis by a specified number of degrees.
    void rotateV(float degrees);

    // Rotates the camera about the W axis by a specified number of degrees.
    void rotateW(float degrees);

    // Sets the near and far clip planes for this camera.
    void setClip(float nearPlane, float farPlane);

    // Returns the current u, v, and w vectors respectivly
    glm::vec4 getU() const;
    glm::vec4 getV() const;
    glm::vec4 getW() const;

    glm::vec4 getEye() const;
    glm::mat4x4 getTranslateMatrix() const;
    glm::mat4x4 getRotateMatrix() const;


private:
    // Camera aspect ratio
    float m_aspectRatio;

    // Camera height angle (in degrees)
    float m_heightAngle;

    // Clipping planes
    float m_near, m_far;

    // The current position in x,y,z world coords
    glm::vec4 m_eye;

    // The look and up vectors
    glm::vec4 m_look, m_up;

    // The camera's u, v, and w vectors
    glm::vec4 m_u, m_v, m_w;
};

#endif // CAMERA_H