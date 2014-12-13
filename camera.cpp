#include "camera.h"

Camera::Camera()
{
    // Setup the camera in it's default state

    // Move this camera to a new eye position, and orient the camera's axes given look and up
    // vectors.
    // orientLook(eye, look, up)
    // Position at (2,2,2,1) to start
    // Make the look vector look at the origin
    // Make the up vector (0, 1, 0, 1);
    orientLook(glm::vec4(2.0f, 2.0f, 2.0f, 1.0f),
               glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f),
               glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
               );

    // Set the height angle
    m_heightAngle = 60.0f;

    // Set the aspect ratio
    m_aspectRatio = 1.0f;

    // Init the clipping planes
    setClip(1.0f, 50.0f);

}

void Camera::setAspectRatio(float a)
{

    this->m_aspectRatio = a;

}

glm::mat4x4 Camera::getProjectionMatrix() const
{
    // Scale and then unhinge the perspective matrix
    //glm::mat4x4 projectionMat = getPerspectiveMatrix() * getScaleMatrix();

    //return projectionMat;
    return getPerspectiveMatrix() * getScaleMatrix();

}

glm::mat4x4 Camera::getViewMatrix() const
{
    /*
    // Calculate the translation matrix, transposed from the row major translation matrix
    glm::mat4x4 translate = glm::transpose(glm::mat4x4(
                                               1.0f, 0.0f, 0.0f, -1 * m_eye.x, // Row 1
                                               0.0f, 1.0f, 0.0f, -1 * m_eye.y, // Row 2
                                               0.0f, 0.0f, 1.0f, -1 * m_eye.z, // Row 3
                                               0.0f, 0.0f, 0.0f, 1.0f // Row 4
                                               ));

    // Calculate the rotation matrix to align the axes, transposed from the row major order I provide
    glm::mat4x4 rotate = glm::transpose(glm::mat4x4(
                                            m_u.x, m_u.y, m_u.z, 0.0f, // Row 1
                                            m_v.x, m_v.y, m_v.z, 0.0f, // Row 2
                                            m_w.x, m_w.y, m_w.z, 0.0f, // Row 3
                                            0.0f, 0.0f, 0.0f, 1.0f // Row 4
                                            ));

    // Translate and then rotate
    return rotate * translate;
    */
    return getRotateMatrix() * getTranslateMatrix();
}


glm::mat4x4 Camera::getTranslateMatrix() const
{
    // Calculate the translation matrix, transposed from the row major translation matrix
    return glm::transpose(glm::mat4x4(
                              1.0f, 0.0f, 0.0f, -1 * m_eye.x, // Row 1
                              0.0f, 1.0f, 0.0f, -1 * m_eye.y, // Row 2
                              0.0f, 0.0f, 1.0f, -1 * m_eye.z, // Row 3
                              0.0f, 0.0f, 0.0f, 1.0f // Row 4
                              ));
}

glm::mat4x4 Camera::getRotateMatrix() const
{
    // Calculate the rotation matrix to align the axes, transposed from the row major order I provide
    return glm::transpose(glm::mat4x4(
                              m_u.x, m_u.y, m_u.z, 0.0f, // Row 1
                              m_v.x, m_v.y, m_v.z, 0.0f, // Row 2
                              m_w.x, m_w.y, m_w.z, 0.0f, // Row 3
                              0.0f, 0.0f, 0.0f, 1.0f // Row 4
                              ));
}

glm::mat4x4 Camera::getScaleMatrix() const
{
    // We have to solve for tan (width angle / 2). See the lab guide. The GUI gives us the height angle
    float tanWidthAngleDiv2 = ((2 * m_far * glm::tan(glm::radians(m_heightAngle / 2))) * m_aspectRatio) / (2 * m_far);
    // Could be simplified to this. Check later.
    // float tanWidthAngleDiv2 = glm::tan(m_heightAngle / 2) * m_aspectRatio;

    float scaleX = 1 / (m_far * tanWidthAngleDiv2);
    float scaleY = 1 / (m_far * glm::tan(glm::radians(m_heightAngle / 2)));
    float scaleZ = 1 / m_far;
    // Transpose to column major order, since I wrote this in row major order
    return glm::transpose(glm::mat4x4(
                              scaleX, 0.0f, 0.0f, 0.0f, // Row 1
                              0.0f, scaleY, 0.0f, 0.0f, // Row 2
                              0.0f, 0.0f, scaleZ, 0.0f, // Row 3
                              0.0f, 0.0f, 0.0f, 1.0f // Row 4
                              ));
}

glm::mat4x4 Camera::getPerspectiveMatrix() const
{
    // Calculate the c
    float c = -1.0 * m_near / m_far;

    // Transpose to column major order, since I wrote this in row major order
    return glm::transpose(glm::mat4x4(
                              1.0f, 0.0f, 0.0f, 0.0f, // Row 1
                              0.0f, 1.0f, 0.0f, 0.0f, // Row 2
                              0.0f, 0.0f, -1.0 / (1 + c), c / (1 + c), // Row 3
                              0.0f, 0.0f, -1.0f, 0.0f // Row 4
                              ));
}

glm::vec4 Camera::getPosition() const
{
    return m_eye;
}

glm::vec4 Camera::getLook() const
{
    return m_look;
}

glm::vec4 Camera::getUp() const
{
    return m_v;
}

float Camera::getAspectRatio() const
{
    return m_aspectRatio;
}

float Camera::getHeightAngle() const
{
    return m_heightAngle;
}

void Camera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up)
{
    // Save the passed vectors
    m_eye = eye;
    m_look = glm::normalize(look);
    m_up = glm::normalize(up);

    // Setup the u, v, w vectors

    // w = -look normalized
    m_w = glm::normalize(-look);

    // v = up - (up dot w)*w normalized
    m_v = glm::normalize( up - (glm::dot(up, m_w) * m_w) );

    // u = v cross w
    glm::vec3 tempV = glm::vec3(m_v.x, m_v.y, m_v.z);
    glm::vec3 tempW = glm::vec3(m_w.x, m_w.y, m_w.z);

    glm::vec3 tempU = glm::cross(tempV, tempW);

    m_u = glm::vec4(tempU.x, tempU.y, tempU.z, 1.0f);

}

void Camera::setHeightAngle(float h)
{
    m_heightAngle = h;
}

void Camera::translate(const glm::vec4 &v)
{
    // We want to translate the eye point by the passed v
    // v.x is the scalar for u, v.y is for v, v.z is for w.

    // The last element should be 0

    m_eye += v;

    // Update the look, up, and right vectors along with the u, v, w system
    orientLook(m_eye, m_look, m_up);
}

void Camera::rotateU(float degrees)
{
    // Adjust pitch

    // Convert to radians
    float theta = glm::radians(degrees);

    // Store the original states
    glm::vec4 v0 = m_v;
    glm::vec4 w0 = m_w;

    // Perform the rotation
    // u doesn't change
    m_v = v0 * glm::cos(theta) + w0 * glm::sin(theta);
    m_w = -v0 * glm::sin(theta) + w0 * glm::cos(theta);

    // Update the look vector to lay along the -w axis
    m_look = -m_w;
    // Zero out the look vector's w coord
    m_look.w *= 0;

    // Update the up vector to match the new v
    m_up = m_v;
    // Zero out the up vector's w coord
    m_v.w *= 0;
}

void Camera::rotateV(float degrees)
{
    // Adjust yaw

    // Convert to radians
    float theta = glm::radians(degrees);

    // Store the original states
    glm::vec4 u0 = m_u;
    glm::vec4 w0 = m_w;

    // Perform the rotation
    m_u = u0 * glm::cos(theta) - w0 * glm::sin(theta);
    // v doesn't change
    m_w = u0 * glm::sin(theta) + w0 * glm::cos(theta);

    // Update the look vector to lay along the -w axis
    m_look = -m_w;
    // Zero out the look vector's w coord
    m_look.w *= 0;

    // Update the up vector to match the new v
    m_up = m_v;
    // Zero out the up vector's w coord
    m_v.w *= 0;
}

void Camera::rotateW(float degrees)
{
    // Adjust spin

    // Convert to radians
    float theta = glm::radians(degrees);

    // Store the original states
    glm::vec4 u0 = m_u;
    glm::vec4 v0 = m_v;

    // Perform the rotation
    m_u = v0 * glm::sin(theta) + u0 * glm::cos(theta);
    m_v = v0 * glm::cos(theta) - u0 * glm::sin(theta);
    // w doesn't change

    // The look vector should not change

    // Update the up vector to match the new v
    m_up = m_v;
    // Zero out the up vector's w coord
    m_v.w = 0;
}

void Camera::setClip(float nearPlane, float farPlane)
{
    m_near = nearPlane;
    m_far = farPlane;
}

glm::vec4 Camera::getU() const
{
    return this->m_u;
}

glm::vec4 Camera::getV() const
{
    return this->m_v;
}

glm::vec4 Camera::getW() const
{
    return this->m_w;
}

glm::vec4 Camera::getEye() const
{
    return this->m_eye;
}

