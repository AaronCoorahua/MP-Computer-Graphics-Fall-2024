#include "Vyrme_Balargon.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <objects.hpp>
#include <OpenGLUtils.hpp>

Vyrme::Vyrme(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation)
    : _shaderProgramHandle(shaderProgramHandle) {

    _shaderProgramUniformLocations.mvpMtx    = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx = normalMtxUniformLocation;

    _shaderProgramUniformLocations.materialAmbientColor  = glGetUniformLocation(_shaderProgramHandle, "materialAmbientColor");
    _shaderProgramUniformLocations.materialDiffuseColor  = glGetUniformLocation(_shaderProgramHandle, "materialDiffuseColor");
    _shaderProgramUniformLocations.materialSpecularColor = glGetUniformLocation(_shaderProgramHandle, "materialSpecularColor");
    _shaderProgramUniformLocations.materialShininess     = glGetUniformLocation(_shaderProgramHandle, "materialShininess");


    _colorBody = glm::vec3(0.3f, 0.0f, 0.3f);
    _colorHead = glm::vec3(1.0f, 1.0f, 1.0f);
    _colorFace = glm::vec3(0.3f, 0.3f, 0.3f);
    _colorBag  = glm::vec3(0.6f, 0.6f, 0.6f);


    position = glm::vec3(0.0f);
}

void Vyrme::drawVehicle(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {

    modelMtx = glm::translate(modelMtx, position);
    modelMtx = glm::rotate(modelMtx, rotationAngle, CSCI441::Y_AXIS);

    _drawBody(modelMtx, viewMtx, projMtx);
    _drawArmRight(modelMtx, viewMtx, projMtx);
    _drawArmLeft(modelMtx, viewMtx, projMtx);
    _drawHead(modelMtx, viewMtx, projMtx);
    _drawFace(modelMtx, viewMtx, projMtx);
    _drawCones(modelMtx, viewMtx, projMtx);
    _drawBag(modelMtx, viewMtx, projMtx);
}

void Vyrme::moveForward() {

    if (_leftArmSwingForward) {
        _leftArmAngle += _armSwingSpeed;
        if (_leftArmAngle >= _armSwingLimit) {
            _leftArmSwingForward = false;
        }
    } else {
        _leftArmAngle -= _armSwingSpeed;
        if (_leftArmAngle <= -_armSwingLimit) {
            _leftArmSwingForward = true;
        }
    }

    if (_rightArmSwingForward) {
        _rightArmAngle += _armSwingSpeed;
        if (_rightArmAngle >= _armSwingLimit) {
            _rightArmSwingForward = false;
        }
    } else {
        _rightArmAngle -= _armSwingSpeed;
        if (_rightArmAngle <= -_armSwingLimit) {
            _rightArmSwingForward = true;
        }
    }
}

void Vyrme::moveBackward() {
    if (_leftArmSwingForward) {
        _leftArmAngle += _armSwingSpeed;
        if (_leftArmAngle >= _armSwingLimit) {
            _leftArmSwingForward = false;
        }
    } else {
        _leftArmAngle -= _armSwingSpeed;
        if (_leftArmAngle <= -_armSwingLimit) {
            _leftArmSwingForward = true;
        }
    }

    if (_rightArmSwingForward) {
        _rightArmAngle += _armSwingSpeed;
        if (_rightArmAngle >= _armSwingLimit) {
            _rightArmSwingForward = false;
        }
    } else {
        _rightArmAngle -= _armSwingSpeed;
        if (_rightArmAngle <= -_armSwingLimit) {
            _rightArmSwingForward = true;
        }
    }

}
void Vyrme::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 bodyMtx = modelMtx;
    bodyMtx = glm::scale(bodyMtx, glm::vec3(0.8f, 2.0f, 0.5f));

    _computeAndSendMatrixUniforms(bodyMtx, viewMtx, projMtx);

    _setMaterialColors(_colorBody, 32.0f);

    CSCI441::drawSolidCube(1.0f);
}

void Vyrme::_drawArmLeft(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 armMtx = modelMtx;

    armMtx = glm::translate(armMtx, glm::vec3(-0.55f, 0.7f, 0.0f));

    armMtx = glm::rotate(armMtx, _leftArmAngle, CSCI441::X_AXIS);

    armMtx = glm::translate(armMtx, glm::vec3(0.0f, -0.7f, 0.0f));

    armMtx = glm::scale(armMtx, glm::vec3(0.30f, 0.9f, 0.3f));
    _computeAndSendMatrixUniforms(armMtx, viewMtx, projMtx);
    _setMaterialColors(_colorBody, 32.0f);
    CSCI441::drawSolidCube(1.0f);
}

void Vyrme::_drawArmRight(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 armMtx = modelMtx;

    armMtx = glm::translate(armMtx, glm::vec3(0.55f, 0.7f, 0.0f));

    armMtx = glm::rotate(armMtx, _rightArmAngle, CSCI441::X_AXIS);

    armMtx = glm::translate(armMtx, glm::vec3(0.0f, -0.7f, 0.0f));

    armMtx = glm::scale(armMtx, glm::vec3(0.30f, 0.9f, 0.3f));

    _computeAndSendMatrixUniforms(armMtx, viewMtx, projMtx);

    _setMaterialColors(_colorBody, 32.0f);

    CSCI441::drawSolidCube(1.0f);
}

void Vyrme::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 headMtx = modelMtx;
    headMtx = glm::translate(headMtx, glm::vec3(0.0f, 1.1f, 0.0f));
    headMtx = glm::scale(headMtx, glm::vec3(0.8f));

    _computeAndSendMatrixUniforms(headMtx, viewMtx, projMtx);

    _setMaterialColors(_colorHead, 64.0f);

    CSCI441::drawSolidSphere(1.0f, 20, 20);
}

void Vyrme::_drawFace(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 faceMtx = modelMtx;
    faceMtx = glm::translate(faceMtx, glm::vec3(0.0f, 1.1f, -0.18f));
    faceMtx = glm::scale(faceMtx, glm::vec3(0.7f));

    _computeAndSendMatrixUniforms(faceMtx, viewMtx, projMtx);

    _setMaterialColors(_colorFace, 32.0f);

    CSCI441::drawSolidSphere(1.0f, 20, 20);
}

void Vyrme::_drawCones(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {

    glm::mat4 coneRightMtx = modelMtx;
    coneRightMtx = glm::translate(coneRightMtx, glm::vec3(0.7f, 1.0f, 0.0f));
    coneRightMtx = glm::rotate(coneRightMtx, glm::radians(-90.0f), CSCI441::Z_AXIS);
    coneRightMtx = glm::scale(coneRightMtx, glm::vec3(0.25f, 0.6f, 0.25f));

    _computeAndSendMatrixUniforms(coneRightMtx, viewMtx, projMtx);

    _setMaterialColors(_colorFace, 32.0f);

    CSCI441::drawSolidCone(1.0f, 1.0f, 20, 20);


    glm::mat4 coneLeftMtx = modelMtx;
    coneLeftMtx = glm::translate(coneLeftMtx, glm::vec3(-0.7f, 1.0f, 0.0f));
    coneLeftMtx = glm::rotate(coneLeftMtx, glm::radians(90.0f), CSCI441::Z_AXIS);
    coneLeftMtx = glm::scale(coneLeftMtx, glm::vec3(0.25f, 0.6f, 0.25f));

    _computeAndSendMatrixUniforms(coneLeftMtx, viewMtx, projMtx);

    _setMaterialColors(_colorFace, 32.0f);

    CSCI441::drawSolidCone(1.0f, 1.0f, 20, 20);
}

void Vyrme::_drawBag(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 bagMtx = modelMtx;
    bagMtx = glm::translate(bagMtx, glm::vec3(0.0f, 0.0f, 0.35f));
    bagMtx = glm::scale(bagMtx, glm::vec3(0.4f, 0.6f, 0.3f));

    _computeAndSendMatrixUniforms(bagMtx, viewMtx, projMtx);

    _setMaterialColors(_colorBag, 32.0f);

    CSCI441::drawSolidCube(1.0f);
}

void Vyrme::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx,
                                          glm::mat4 projMtx) const {
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1,
                              GL_FALSE, glm::value_ptr(mvpMtx));

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glProgramUniformMatrix3fv(_shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1,
                              GL_FALSE, glm::value_ptr(normalMtx));
}

void Vyrme::_setMaterialColors(glm::vec3 color, float shininess) const {
    glm::vec3 ambientColor  = color * 0.2f;
    glm::vec3 diffuseColor  = color;
    glm::vec3 specularColor = glm::vec3(0.5f);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialAmbientColor, 1,
                        glm::value_ptr(ambientColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialDiffuseColor, 1,
                        glm::value_ptr(diffuseColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialSpecularColor, 1,
                        glm::value_ptr(specularColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.materialShininess, shininess);
}
