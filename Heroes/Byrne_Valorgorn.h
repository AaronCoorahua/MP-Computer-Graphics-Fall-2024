//
// Author: Seth Motta
//

#ifndef A3_HERO_H
#define A3_HERO_H

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Byrne_Valorgorn {
public:
    Byrne_Valorgorn(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation ); // Constructor
    void drawHero( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ); // Draw all pieces of the Hero
    void move(float[]); // Move the Hero
    void rotate(); // Rotate the Hero

    // Location and Angle
    glm::vec3 getHeroLocation();
    GLfloat getRotationAngle();

    // Whether or not the Hero is moving forward and turning left (counter-clockwise)
    bool _movingForward;
    bool _turningLeft;
    GLfloat _armRotation;

private:
    // Shader information
    GLuint _shaderProgramHandle;

    struct ShaderProgramUniformLocations {
        GLint mvpMtx;
        GLint normalMtx;
        GLint ambientMaterialColor;
        GLint diffuseMaterialColor;
        GLint specularMaterialColor;
        GLint shineMaterial;
    } _shaderProgramUniformLocations{};

    // Hero colors
    glm::vec3 _colorHead{};
    glm::vec3 _colorHair{};
    glm::vec3 _colorArmor{};

    // Hero location and direction (heading)
    glm::vec3 _heroLocation{};
    glm::vec3 _heroDirection{};

    // Hero speed and rotation
    GLfloat _speed;
    GLfloat _rotateAngle;

    // Draw each component of the Hero
    void _drawHeroHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroHairBack(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroHairTop(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroLeftEar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroRightEar(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroLeftArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;
    void _drawHeroRightArm(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};

#endif //A3_HERO_H
