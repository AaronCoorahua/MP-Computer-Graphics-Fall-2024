#ifndef ARCBALLCAM_H
#define ARCBALLCAM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ArcballCam {
public:
    ArcballCam();
    ArcballCam(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    glm::vec3 getUpVector() const;
    glm::vec3 getLookAtPoint() const;
    glm::vec3 getViewDir() const;
    glm::vec3 getRightVector() const;

    void setCameraView(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up);
    void updateViewMatrix();

    void rotate(float deltaX, float deltaY, int viewportWidth, int viewportHeight);
    void zoom(float amount);
    void setLookAtPoint(const glm::vec3& lookAt);

private:

    glm::mat4 mViewMatrix;
    glm::vec3 mEye;
    glm::vec3 mLookAt;
    glm::vec3 mUp;

    float mTheta;
    float mPhi;
    float mRadius;
};

#endif // ARCBALLCAM_H
