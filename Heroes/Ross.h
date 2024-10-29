#ifndef Ross_h
#define Ross_h

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Ross {
public:
    Ross(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation);

    void drawVehicle(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);

    void moveForward();
    void moveBackward();

private:
    float movementSpeed = 0.1f;
    float rotationSpeed = glm::radians(5.0f);
    bool isMovingForward = false;

    GLuint _shaderProgramHandle;
    struct ShaderProgramUniformLocations {
        GLint mvpMtx;
        GLint normalMtx;
        GLint materialAmbientColor;
        GLint materialDiffuseColor;
        GLint materialSpecularColor;
        GLint materialShininess;
    } _shaderProgramUniformLocations;

    // Wizard-specific variables
    glm::vec3 _colorBody;
    glm::vec3 _scaleBody;

    glm::vec3 _colorHead;
    glm::vec3 _scaleHead;
    glm::vec3 _transHead;

    glm::vec3 _colorHat;
    glm::vec3 _scaleHat;
    glm::vec3 _transHat;

    glm::vec3 _colorBeard;
    glm::vec3 _scaleBeard;
    glm::vec3 _transBeard;
    float _angleBeard;

    glm::vec3 _colorStaff;
    glm::vec3 _scaleStaff;
    glm::vec3 _transStaff;
    float _staffAngle;

    // Position (if needed)
    glm::vec3 position;

    // Constants
    const GLfloat _PI = glm::pi<float>();
    const GLfloat _2PI = glm::two_pi<float>();
    const GLfloat _PI_OVER_2 = glm::half_pi<float>();

    // Drawing functions
    void _drawBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawHat(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawBeard(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _drawStaff(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
    void _setMaterialColors(glm::vec3 color, float shininess) const;
};

#endif // Ross_h
