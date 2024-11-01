#ifndef MP_ENGINE_H
#define MP_ENGINE_H

#include "Cameras/ArcballCam.h"
#include <OpenGLEngine.hpp>
#include <ShaderProgram.hpp>
#include "FreeCam.hpp"
#include "FixedCam.hpp"

#include "Heroes/Aaron_Inti.h"
#include "Heroes/Ross.h"
#include "Heroes/Vyrme_Balargon.h"

#include "stb_image.h"
#include <glad/gl.h>
#include <vector>
#include <string>

#include <vector>

class MP final : public CSCI441::OpenGLEngine {
public:
    MP();
    MP(const std::string& animationFilePath);
    ~MP() final;

    void run() final;

    // Handle key events inside the engine
    void handleKeyEvent(GLint key, GLint action);

    // Handle mouse button events inside the engine
    void handleMouseButtonEvent(GLint button, GLint action);

    // Handle cursor movement events inside the engine
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
    struct CameraFrame {
        glm::vec3 eye;
        glm::vec3 direction;
        glm::vec3 up;
        float fov;
    };

    // Characters
    enum Character { AARON_INTI, ROSS, VYRME };
    Character _selectedCharacter;

    glm::mat4 _projectionMatrix;
    glm::vec3 _planePosition;
    float _planeHeading;

    glm::vec3 _rossPosition;
    float _rossHeading;

    glm::vec3 _vyrmePosition;
    float _vyrmeHeading;

    void mSetupGLFW() final;
    void mSetupOpenGL() final;
    void mSetupShaders() final;
    void mSetupBuffers() final;
    void mSetupScene() final;

    void mCleanupBuffers() final;
    void mCleanupShaders() final;

    // Draws the scene from a specific camera viewpoint
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx, glm::vec3 eyePosition) const;

    // Update scene elements based on time and input
    void _updateScene(float deltaTime);

    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    GLboolean _keys[NUM_KEYS];

    glm::vec2 _mousePosition;
    GLint _leftMouseButtonState;

    enum CameraMode { ARCBALL, FREE_CAM, FIRST_PERSON_CAM, ANIMATED_CAM } _currentCameraMode;

    ArcballCam* _arcballCam;
    CSCI441::FreeCam* _freeCam;
    CSCI441::FreeCam* _intiFirstPersonCam;
    CSCI441::FreeCam* _rossFirstPersonCam;
    CSCI441::FreeCam* _vyrmeFirstPersonCam;
    glm::vec2 _cameraSpeed;

    Aaron_Inti* _pPlane;
    Ross* _rossHero;
    Vyrme* _vyrmeHero;

    static constexpr GLfloat WORLD_SIZE = 55.0f;
    GLuint _groundVAO;
    GLsizei _numGroundPoints;

    void _createGroundBuffers();

    struct BuildingData {
        glm::mat4 modelMatrix;
        glm::vec3 color;
    };
    std::vector<BuildingData> _buildings;

    void _generateEnvironment();

    CSCI441::ShaderProgram* _lightingShaderProgram = nullptr;

    struct LightingShaderUniformLocations {
        GLint mvpMatrix;
        GLint normalMatrix;
        GLint eyePosition;

        GLint lightDirection;
        GLint lightAmbientColor;
        GLint lightDiffuseColor;
        GLint lightSpecularColor;

        GLint materialAmbientColor;
        GLint materialDiffuseColor;
        GLint materialSpecularColor;
        GLint materialShininess;

        GLint pointLightPos;
        GLint pointLightColor;
        GLint pointLightConstant;
        GLint pointLightLinear;
        GLint pointLightQuadratic;

        GLint spotLightPos;
        GLint spotLightDirection;
        GLint spotLightColor;
        GLint spotLightCutoff;
        GLint spotLightOuterCutoff;
        GLint spotLightExponent;
        GLint spotLightConstant;
        GLint spotLightLinear;
        GLint spotLightQuadratic;
    } _lightingShaderUniformLocations;

    struct LightingShaderAttributeLocations {
        GLint vPos;
        GLint vNormal;
    } _lightingShaderAttributeLocations;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

    struct TreeData {
        glm::mat4 modelMatrix;
    };

    struct RockData {
        glm::mat4 modelMatrix;
        float scale;
    };

    std::vector<TreeData> _trees;
    std::vector<RockData> _rocks;

    bool _isShiftPressed;
    bool _isLeftMouseButtonPressed;
    bool _isZooming;

    glm::vec2 _prevMousePosition;

    void _updateIntiFirstPersonCamera();
    void _updateRossFirstPersonCamera();
    void _updateVyrmeFirstPersonCamera();

    // SKYBOX

    bool _isSmallViewportActive;
    Character _smallViewportCharacter;
    static constexpr int NUM_CHARACTERS = 3;

    GLuint _skyboxVAO, _skyboxVBO;
    GLuint _skyboxTexture;
    CSCI441::ShaderProgram* _skyboxShaderProgram = nullptr;

    GLuint loadCubemap(const std::vector<std::string>& faces);
    void _setupSkybox();


    //CAMERA ANIMATION

    std::vector<CameraFrame> _cameraAnimationFrames;
    bool _isAnimating = false;
    size_t _currentAnimationFrame = 0;
    float _animationTime = 0.0f;
    float _frameDuration = 1.0f / 30.0f;

    bool _loadCameraAnimation(const std::string& filename);
};

void A3_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void A3_engine_cursor_callback(GLFWwindow *window, double x, double y );
void A3_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif MP_ENGINE_H
