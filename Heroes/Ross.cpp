#include "Ross.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

Ross::Ross( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {

    _propAngle = 0.0f;
    _propAngleRotationSpeed = _PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = _PI / 2.0f;

    _colorBody = glm::vec3( 0.5f, 0.5f, 0.5f );
    _scaleBody = glm::vec3( 0.055f, 0.055f, 0.055f );

    _colorHead = glm::vec3( 0.941f, 0.725f, 0.2f );
    _scaleHead = glm::vec3( 0.04f, 0.06f, 0.04f );
    _transHead = glm::vec3( 0.0f, 0.22f, 0.0f );

    _colorHat = _colorBody;
    _scaleHat = glm::vec3( 0.02f, 0.02f, 0.02f );
    _transHat = glm::vec3( 0.0f, 0.24f, 0.0f );

    _colorBeard = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleBeard = glm::vec3( 0.03f, 0.03f, 0.03f );
    _transBeard = glm::vec3( 0.01f, 0.19f, 0.0f );
    _angleBeard = 60.0f;

    _colorProp = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleProp = glm::vec3( 1.1f, 1.0f, 0.025f );
    _transProp = glm::vec3( 0.1f, 0.0f, 0.0f );

    _colorStaff = glm::vec3( 0.54f, 0.46f, 0.37f );
    _scaleStaff = glm::vec3( 0.08f, 1.9f, 0.08f );
    _transStaff = glm::vec3( 0.0f, 0.15f, 0.05f );
    _staffAngle = 0.0f;

}

void Ross::drawRoss( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx,GLfloat time ) {

    // Translate to the wizard's position
   // modelMtx = glm::translate(modelMtx, position);

    // Rotate according to the wizard's heading
    glm::vec3 upVector(0, 1, 0); // Assuming the up vector is Y-axis
    GLfloat angle = - atan2(heading.z, heading.x); // Calculate the angle in the XZ plane
    modelMtx = glm::rotate(modelMtx, angle, upVector);


    //Draw individual components
    _drawBody(modelMtx, viewMtx, projMtx);        // the body
    _drawWHead(modelMtx, viewMtx, projMtx);        // the head
    _drawHat(modelMtx, viewMtx, projMtx);
    _drawBeard(modelMtx, viewMtx, projMtx);
    _drawStaff(modelMtx, viewMtx, projMtx, time);
}

void Ross::rotateLeft() {
    // Use transformation to rotate the heading
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotateAngle, glm::vec3(0, 1, 0));
    glm::vec4 newHeading = rotation * glm::vec4(heading, 0.0f);

    heading = glm::vec3(newHeading); // Convert back to vec3
    heading = glm::normalize(heading);
}

void Ross::rotateRight() {
    // Use transformation to rotate the heading
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), - rotateAngle, glm::vec3(0, 1, 0));
    glm::vec4 newHeading = rotation * glm::vec4(heading, 0.0f);

    heading = glm::vec3(newHeading); // Convert back to vec3
    heading = glm::normalize(heading);
}



void Ross::_drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBody));

    CSCI441::drawSolidCone(0.8f,5.0f,20,20);
}

void Ross::_drawWHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate( modelMtx, _transHead );

    modelMtx = glm::scale( modelMtx, _scaleHead );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorHead));

    CSCI441::drawSolidSphere(1.0,20,20);
}


void Ross::_drawHat(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate( modelMtx, _transHat );

    modelMtx = glm::scale( modelMtx, _scaleHat );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorHat));

    CSCI441::drawSolidCone(3.0f,7.0f,20,20);

}

void Ross::_drawBeard(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transBeard );
    modelMtx1 = glm::rotate( modelMtx1, _angleBeard, CSCI441::Z_AXIS );
    modelMtx1 = glm::scale( modelMtx1, _scaleBeard );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBeard));

    CSCI441::drawSolidCone(1.0f,2.0f,20,20);

}



void Ross::_drawStaff(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx , GLfloat time) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transStaff );
    modelMtx1 = glm::rotate( modelMtx1, _staffAngle +(time*2), CSCI441::Z_AXIS );
    modelMtx1 = glm::scale( modelMtx1, _scaleStaff);

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorStaff));

    CSCI441::drawSolidCube( 0.1f );
}


void Ross::moveForward(GLfloat WORLD_SIZE) {
    WORLD_SIZE = WORLD_SIZE - 0.5;
    glm::vec3 newPosition = position + heading * 0.5f;
    if (newPosition.x >= WORLD_SIZE){
        newPosition.x = WORLD_SIZE;
    } else if (newPosition.x <= -WORLD_SIZE) {
        newPosition.x = -WORLD_SIZE;
    }
    if (newPosition.z >= WORLD_SIZE) {
        newPosition.z = WORLD_SIZE;
    } else if (newPosition.z <= -WORLD_SIZE) {
        newPosition.z = -WORLD_SIZE;
    }

    position = newPosition;

}

void Ross::moveBackward(GLfloat WORLD_SIZE) {
    WORLD_SIZE = WORLD_SIZE - 0.5;
    glm::vec3 newPosition = position - heading * 0.5f;
    if (newPosition.x >= WORLD_SIZE){
        newPosition.x = WORLD_SIZE;
    } else if (newPosition.x <= -WORLD_SIZE) {
        newPosition.x = -WORLD_SIZE;
    }
    if (newPosition.z >= WORLD_SIZE) {
        newPosition.z = WORLD_SIZE;
    } else if (newPosition.z <= -WORLD_SIZE) {
        newPosition.z = -WORLD_SIZE;
    }

    position = newPosition;
}

void Ross::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx) );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx) );
}

void Ross::_setPosition(glm::vec3 inPosition) {
    position = inPosition;
}

glm::vec3 Ross::_getPosition() const {
    return position;
}