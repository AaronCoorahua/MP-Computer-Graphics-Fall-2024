//
// Author: Seth Motta
//

#include "Byrne_Valorgorn.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <objects.hpp>
#include <OpenGLUtils.hpp>

Byrne_Valorgorn::Byrne_Valorgorn(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation) {
    // Shader program and info
    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;

    _shaderProgramUniformLocations.ambientMaterialColor = glGetUniformLocation(_shaderProgramHandle, "ambientMaterialColor");
    _shaderProgramUniformLocations.diffuseMaterialColor = glGetUniformLocation(_shaderProgramHandle, "diffuseMaterialColor");
    _shaderProgramUniformLocations.specularMaterialColor = glGetUniformLocation(_shaderProgramHandle, "specularMaterialColor");
    _shaderProgramUniformLocations.shineMaterial = glGetUniformLocation(_shaderProgramHandle, "shineMaterial");

    // Hero colors
    _colorHead = glm::vec3(0.37, 0.39, 0.47);
    _colorHair = glm::vec3(1.0,1.0,1.0);
    _colorArmor = glm::vec3(0.31f, 0.12f, 0.55f);

    // Hero location and direction (heading)
    _heroLocation = glm::vec3(0.0f, 1.0f, 0.0f);
    _heroDirection = glm::vec3(0.0f, 0.0f, 0.0f);

    // Hero speed and rotation
    _speed = 0.5f;
    _rotateAngle = 0.0f;
    _armRotation = 0.0f;

    // Whether moving forward and turning left (counter-clockwise)
    _movingForward = false;
    _turningLeft = false;
}

// Move Hero relative to its heading and based on its speed
// Ensure it can't move beyond the boundaries of the scene
void Byrne_Valorgorn::move(float boundaries[]) {
    glm::vec3 newLocation;
    _heroDirection = {sin(_rotateAngle), 0.0f, cos(_rotateAngle)};

    if(_movingForward) {
        newLocation = _heroLocation + _heroDirection * _speed;
    } else {
        newLocation = _heroLocation - _heroDirection * _speed;
    }

    newLocation.x = std::max(boundaries[0], std::min(newLocation.x, boundaries[1]));
    newLocation.z = std::max(boundaries[0], std::min(newLocation.z, boundaries[1]));

    _heroLocation = newLocation;
}

// Rotate Hero, changing its heading
void Byrne_Valorgorn::rotate() {
    if(_turningLeft) {
        _rotateAngle += M_PI / 50.0f;
    } else {
        _rotateAngle -= M_PI / 50.0f;
    }
}

glm::vec3 Byrne_Valorgorn::getHeroLocation() {
    return _heroLocation;
}

GLfloat Byrne_Valorgorn::getRotationAngle() {
    return _rotateAngle;
}

// Draw each component of the Hero
void Byrne_Valorgorn::drawHero(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    modelMtx = glm::translate(modelMtx, _heroLocation);
    modelMtx = glm::rotate(modelMtx, _rotateAngle, CSCI441::Y_AXIS);

    _drawHeroBody(modelMtx, viewMtx, projMtx);
    _drawHeroHead(modelMtx, viewMtx, projMtx);
    _drawHeroHairBack(modelMtx, viewMtx,projMtx);
    _drawHeroHairTop(modelMtx, viewMtx,projMtx);
    _drawHeroLeftEar(modelMtx, viewMtx, projMtx);
    _drawHeroRightEar(modelMtx, viewMtx, projMtx);
    _drawHeroLeftArm(modelMtx, viewMtx, projMtx);
    _drawHeroRightArm(modelMtx, viewMtx, projMtx);
}

// Draw the Hero's head with a grey sphere
void Byrne_Valorgorn::_drawHeroHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0, 1.5f, 0.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(7.5f, 7.5f, 7.5f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorHead * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorHead * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float headShine = 12.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, headShine);

    CSCI441::drawSolidSphere(0.1, 40, 40);
}

// Draw the back of the Hero's hair with a white cylinder
void Byrne_Valorgorn::_drawHeroHairBack(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0, -0.01f, -0.335f));
    modelMtx = glm::scale(modelMtx, glm::vec3(5.6f, 16.0f, 5.6));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorHair * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorHair * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float hairShine = 4.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, hairShine);

    CSCI441::drawSolidCylinder(0.1, 0.1, 0.1, 40, 40);
}

// Draw the top of the Hero's hair with a white sphere
void Byrne_Valorgorn::_drawHeroHairTop(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0, 1.5f, -0.1f));
    modelMtx = glm::scale(modelMtx, glm::vec3(8.0f, 8.0f, 8.0f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorHair * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorHair * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float hairShine = 4.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, hairShine);

    CSCI441::drawSolidSphere(0.1, 40, 40);
}

// Draw the Hero's left ear with a gray cone
void Byrne_Valorgorn::_drawHeroLeftEar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(-0.6f, 1.5f, 0.0f));
    modelMtx = glm::rotate(modelMtx, glm::radians(90.0f), CSCI441::Z_AXIS);
    modelMtx = glm::scale(modelMtx, glm::vec3(1.0f, 8.0f, 1.0f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorHead * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorHead * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float hairShine = 4.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, hairShine);

    CSCI441::drawSolidCone(0.25, 0.1, 40, 40);
}

// Draw the Hero's right ear with a gray cone
void Byrne_Valorgorn::_drawHeroRightEar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.6f, 1.5f, 0.0f));
    modelMtx = glm::rotate(modelMtx, glm::radians(270.0f), CSCI441::Z_AXIS);
    modelMtx = glm::scale(modelMtx, glm::vec3(1.0f, 8.0f, 1.0f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorHead * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorHead * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float hairShine = 4.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, hairShine);

    CSCI441::drawSolidCone(0.25, 0.1, 40, 40);
}

// Draw the Hero's body with a purple cube
void Byrne_Valorgorn::_drawHeroBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::scale(modelMtx, glm::vec3(10.0f, 18.0f, 5.0f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorArmor * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorArmor * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float armorShine = 24.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, armorShine);

    CSCI441::drawSolidCube(0.1f);
}

// Draw the Hero's left arm with a purple cube
void Byrne_Valorgorn::_drawHeroLeftArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.75f, 0.3f, 0.0f));
    modelMtx = glm::rotate(modelMtx, _armRotation / 2, CSCI441::X_AXIS); // TODO: Fix arm swing animation
    modelMtx = glm::scale(modelMtx, glm::vec3(5.0f, 12.0f, 2.5f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorArmor * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorArmor * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float armorShine = 24.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, armorShine);

    CSCI441::drawSolidCube(0.1f);
}

// Draw the Hero's right arm with a purple cube
void Byrne_Valorgorn::_drawHeroRightArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(-0.75f, 0.3f, 0.0f));
    modelMtx = glm::rotate(modelMtx, _armRotation / 2, CSCI441::X_AXIS); // TODO: Fix arm swing animation
    modelMtx = glm::scale(modelMtx, glm::vec3(5.0f, 12.0f, 2.5f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glm::vec3 ambientMaterialColor = _colorArmor * 0.15f;
    glm::vec3 diffuseMaterialColor = _colorArmor * 1.0f;
    glm::vec3 specularMaterialColor(0.35f, 0.35f, 0.35f);
    float armorShine = 24.0f;

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.ambientMaterialColor, 1, glm::value_ptr(ambientMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.diffuseMaterialColor, 1, glm::value_ptr(diffuseMaterialColor));
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.specularMaterialColor, 1, glm::value_ptr(specularMaterialColor));
    glProgramUniform1f(_shaderProgramHandle, _shaderProgramUniformLocations.shineMaterial, armorShine);

    CSCI441::drawSolidCube(0.1f);
}

void Byrne_Valorgorn::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx) );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx) );
}


