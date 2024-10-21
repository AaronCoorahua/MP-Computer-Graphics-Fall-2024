#include "ArcballCam.h"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

ArcballCam::ArcballCam()
    : mLookAt(0.0f, 0.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f) {
    mRadius = 50.0f; // Set an initial radius
    mTheta = glm::radians(45.0f);
    mPhi = glm::radians(45.0f);
    updateViewMatrix();
}

ArcballCam::ArcballCam(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up)
    : mLookAt(std::move(lookAt)), mUp(std::move(up)) {
    glm::vec3 offset = eye - mLookAt;
    mRadius = glm::length(offset);
    mTheta = atan2(offset.x, offset.z);
    mPhi = acos(offset.y / mRadius);
    updateViewMatrix();
}

void ArcballCam::updateViewMatrix() {
    // Ensure phi is within valid range to prevent flipping
    const float epsilon = 0.001f;
    if (mPhi <= epsilon) mPhi = epsilon;
    if (mPhi >= glm::pi<float>() - epsilon) mPhi = glm::pi<float>() - epsilon;

    // Compute mEye based on mLookAt, mTheta, mPhi, mRadius
    mEye.x = mLookAt.x + mRadius * sinf(mPhi) * sinf(mTheta);
    mEye.y = mLookAt.y + mRadius * cosf(mPhi);
    mEye.z = mLookAt.z + mRadius * sinf(mPhi) * cosf(mTheta);

    mViewMatrix = glm::lookAt(mEye, mLookAt, mUp);
}

void ArcballCam::rotate(float deltaX, float deltaY, int viewportWidth, int viewportHeight) {
    float deltaAngleX = glm::two_pi<float>() / viewportWidth;
    float deltaAngleY = glm::pi<float>() / viewportHeight;

    float xAngle = deltaX * deltaAngleX;
    float yAngle = deltaY * deltaAngleY;

    mTheta += xAngle;
    mPhi += yAngle;

    // Keep theta in [0, 2π]
    if (mTheta > glm::two_pi<float>())
        mTheta -= glm::two_pi<float>();
    else if (mTheta < 0.0f)
        mTheta += glm::two_pi<float>();

    // Clamp phi to prevent flipping at poles
    const float epsilon = 0.001f;
    mPhi = glm::clamp(mPhi, epsilon, glm::pi<float>() - epsilon);

    updateViewMatrix();
}

void ArcballCam::zoom(float amount) {
    mRadius += amount;
    if (mRadius < 1.0f) mRadius = 1.0f; // Allow closer zoom
    if (mRadius > 500.0f) mRadius = 500.0f; // Allow further zoom out
    updateViewMatrix();
}

void ArcballCam::setCameraView(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up) {
    mLookAt = std::move(lookAt);
    mUp = std::move(up);
    glm::vec3 offset = eye - mLookAt;
    mRadius = glm::length(offset);
    mTheta = atan2(offset.x, offset.z);
    mPhi = acos(offset.y / mRadius);
    updateViewMatrix();
}

glm::mat4 ArcballCam::getViewMatrix() const {
    return mViewMatrix;
}

glm::vec3 ArcballCam::getPosition() const {
    return mEye;
}

glm::vec3 ArcballCam::getUpVector() const {
    return mUp;
}

glm::vec3 ArcballCam::getLookAtPoint() const {
    return mLookAt;
}

glm::vec3 ArcballCam::getViewDir() const {
    return glm::normalize(mLookAt - mEye);
}

glm::vec3 ArcballCam::getRightVector() const {
    return glm::normalize(glm::cross(getViewDir(), mUp));
}

void ArcballCam::setLookAtPoint(const glm::vec3& lookAt) {
    mLookAt = lookAt;
    updateViewMatrix();
}
