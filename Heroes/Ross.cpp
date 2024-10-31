#include "Ross.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../CSCI441/include/objects.hpp"
#include "../CSCI441/include/OpenGLUtils.hpp"

Ross::Ross(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation)
    : _shaderProgramHandle(shaderProgramHandle) {

    _shaderProgramUniformLocations.mvpMtx    = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx = normalMtxUniformLocation;

    _shaderProgramUniformLocations.materialAmbientColor  = glGetUniformLocation(_shaderProgramHandle, "materialAmbientColor");
    _shaderProgramUniformLocations.materialDiffuseColor  = glGetUniformLocation(_shaderProgramHandle, "materialDiffuseColor");
    _shaderProgramUniformLocations.materialSpecularColor = glGetUniformLocation(_shaderProgramHandle, "materialSpecularColor");
    _shaderProgramUniformLocations.materialShininess     = glGetUniformLocation(_shaderProgramHandle, "materialShininess");

    // Initialize wizard-specific variables
    _colorBody = glm::vec3(0.5f, 0.5f, 0.5f);          // Gray color for the body
    _scaleBody = glm::vec3(0.055f, 0.055f, 0.055f);

    _colorHead = glm::vec3(0.941f, 0.725f, 0.2f);      // Skin color for the head
    _scaleHead = glm::vec3(0.04f, 0.06f, 0.04f);
    _transHead = glm::vec3(0.0f, 0.22f, 0.0f);

    _colorHat = _colorBody;                             // Same color as body
    _scaleHat = glm::vec3(0.02f, 0.02f, 0.02f);
    _transHat = glm::vec3(0.0f, 0.24f, 0.0f);

    _colorBeard = glm::vec3(1.0f, 1.0f, 1.0f);          // White color for the beard
    _scaleBeard = glm::vec3(0.03f, 0.03f, 0.03f);
    _transBeard = glm::vec3(0.00f, 0.19f, -0.01f);
    _angleBeard = glm::radians(-150.0f);

    _colorStaff = glm::vec3(0.54f, 0.46f, 0.37f);       // Brown color for the staff
    _scaleStaff = glm::vec3(0.08f, 1.9f, 0.08f);
    _transStaff = glm::vec3(0.04f, 0.15f, 0.01f);
    _staffAngle = 0.0f;

    // Initialize position (if needed)
    position = glm::vec3(0.0f);
}

void Ross::drawVehicle(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {

    float scaleCharacter= 10.0f;
    modelMtx = glm::scale(modelMtx, glm::vec3(scaleCharacter));
    // Draw individual components
    _drawBody(modelMtx, viewMtx, projMtx);        // The body
    _drawHead(modelMtx, viewMtx, projMtx);        // The head
    _drawHat(modelMtx, viewMtx, projMtx);         // The hat
    _drawBeard(modelMtx, viewMtx, projMtx);       // The beard
    _drawStaff(modelMtx, viewMtx, projMtx);       // The staff
}

void Ross::moveForward() {
    isMovingForward = true;
    position.z -= movementSpeed;

    _staffAngle -= rotationSpeed;
    if (_staffAngle > glm::two_pi<float>()) {
        _staffAngle -= glm::two_pi<float>();
    }
}

void Ross::moveBackward() {
    isMovingForward = false;
    position.z += movementSpeed;

    _staffAngle += rotationSpeed;
    if (_staffAngle > glm::two_pi<float>()) {
        _staffAngle -= glm::two_pi<float>();
    }
}

void Ross::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 bodyMtx = modelMtx;
    bodyMtx = glm::scale(bodyMtx, _scaleBody);

    _computeAndSendMatrixUniforms(bodyMtx, viewMtx, projMtx);

    _setMaterialColors(_colorBody, 32.0f);

    CSCI441::drawSolidCone(0.8f, 5.0f, 20, 20);
}

void Ross::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 headMtx = modelMtx;
    headMtx = glm::translate(headMtx, _transHead);
    headMtx = glm::scale(headMtx, _scaleHead);

    _computeAndSendMatrixUniforms(headMtx, viewMtx, projMtx);

    _setMaterialColors(_colorHead, 32.0f);

    CSCI441::drawSolidSphere(1.0f, 20, 20);
}

void Ross::_drawHat(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 hatMtx = modelMtx;
    hatMtx = glm::translate(hatMtx, _transHat);
    hatMtx = glm::scale(hatMtx, _scaleHat);

    _computeAndSendMatrixUniforms(hatMtx, viewMtx, projMtx);

    _setMaterialColors(_colorHat, 32.0f);

    CSCI441::drawSolidCone(3.0f, 7.0f, 20, 20);
}

void Ross::_drawBeard(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 beardMtx = modelMtx;
    beardMtx = glm::translate(beardMtx, _transBeard);
    beardMtx = glm::rotate(beardMtx, _angleBeard, CSCI441::X_AXIS);
    beardMtx = glm::scale(beardMtx, _scaleBeard);

    _computeAndSendMatrixUniforms(beardMtx, viewMtx, projMtx);

    _setMaterialColors(_colorBeard, 32.0f);

    CSCI441::drawSolidCone(1.0f, 2.0f, 20, 20);
}

void Ross::_drawStaff(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const{
    glm::mat4 staffMtx = modelMtx;
    staffMtx = glm::translate(staffMtx, _transStaff);

    if (isMovingForward) {
        staffMtx = glm::rotate(staffMtx, _staffAngle, CSCI441::X_AXIS);
    } else {

        staffMtx = glm::rotate(staffMtx, _staffAngle, CSCI441::X_AXIS);
    }

    staffMtx = glm::scale(staffMtx, _scaleStaff);

    _computeAndSendMatrixUniforms(staffMtx, viewMtx, projMtx);

    _setMaterialColors(_colorStaff, 32.0f);

    CSCI441::drawSolidCube(0.1f);
}


void Ross::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    glProgramUniformMatrix4fv(_shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx));

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    glProgramUniformMatrix3fv(_shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx));
}

void Ross::_setMaterialColors(glm::vec3 color, float shininess) const {
    glm::vec3 ambientColor  = color * 0.2f;
    glm::vec3 diffuseColor  = color;
    glm::vec3 specularColor = glm::vec3(0.5f);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialAmbientColor, 1, glm::value_ptr(ambientColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialDiffuseColor, 1, glm::value_ptr(diffuseColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialSpecularColor, 1, glm::value_ptr(specularColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.materialShininess, shininess);
}