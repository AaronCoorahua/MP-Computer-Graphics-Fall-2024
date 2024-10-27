#ifndef VYRME_H
#define VYRME_H

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Vyrme {
public:
    Vyrme(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation);

    void drawVehicle(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);

    void moveForward();
    void moveBackward();

    glm::vec3 getPosition() const { return position; }
    float getRotationAngle() const { return rotationAngle; }

private:
    GLuint _shaderProgramHandle;
    struct ShaderProgramUniformLocations {
        GLint mvpMtx;
        GLint normalMtx;
        GLint materialAmbientColor;
        GLint materialDiffuseColor;
        GLint materialSpecularColor;
        GLint materialShininess;
    } _shaderProgramUniformLocations;

    glm::vec3 _colorBody;
    glm::vec3 _colorHead;
    glm::vec3 _colorFace;
    glm::vec3 _colorBag;

    glm::vec3 position;
    float rotationAngle = 0.0f;

    void _drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawArmRight(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawArmLeft(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawFace(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawCones(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawBag(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx,
                                       glm::mat4 projMtx) const;
    void _setMaterialColors(glm::vec3 color, float shininess) const;

    float _leftArmAngle = 0.0f;
    float _rightArmAngle = 0.0f;
    bool _leftArmSwingForward = true;
    bool _rightArmSwingForward = false;
    float _armSwingSpeed = glm::radians(1.0f);
    float _armSwingLimit = glm::radians(30.0f);
};

#endif // VYRME_H
