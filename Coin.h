#ifndef COIN_H
#define COIN_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Coin {
public:
 Coin(GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation);

 void drawCoin(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx);


 void deactivate() { _isActive = false; }

 bool isActive() const { return _isActive; }

private:
 bool _isActive;
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
 glm::vec3 _scaleBody;

 void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
 void _setMaterialColors(glm::vec3 color, float shininess) const;
};

#endif // COIN_H
