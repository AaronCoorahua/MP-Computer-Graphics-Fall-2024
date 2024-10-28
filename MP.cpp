#include "MP.h"

#include <objects.hpp>

#include <glm/gtc/type_ptr.hpp>  // for glm::value_ptr()

#include <ctime>
#include <algorithm>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265f
#endif

/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand() {
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//*************************************************************************************
//
// Public Interface

MP::MP()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "MP: Over Hill and Under Hill"),
                                _isShiftPressed(false),
                                _isLeftMouseButtonPressed(false),
                                _isZooming(false),
                                _currentCameraMode(ARCBALL),
                                _isSmallViewportActive(false),
                                _smallViewportCharacter(AARON_INTI){

    for(auto& _key : _keys) _key = GL_FALSE;
    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    _arcballCam = new ArcballCam();
    _freeCam = new CSCI441::FreeCam();

    _intiFirstPersonCam = new CSCI441::FreeCam();
    _rossFirstPersonCam = new CSCI441::FreeCam();
    _vyrmeFirstPersonCam = new CSCI441::FreeCam();


}

MP::~MP() {
    delete _arcballCam;
    delete _freeCam;
    delete _intiFirstPersonCam;
    delete _rossFirstPersonCam;
    delete _vyrmeFirstPersonCam;
}


void MP::handleKeyEvent(GLint key, GLint action) {
    if (key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
            break;
            case GLFW_KEY_LEFT_SHIFT:
            case GLFW_KEY_RIGHT_SHIFT:
                _isShiftPressed = true;
            break;
            case GLFW_KEY_Z:
                _selectedCharacter = AARON_INTI;
            _currentCameraMode = ARCBALL;
            _arcballCam->setLookAtPoint(_planePosition);
            break;
            case GLFW_KEY_X:
                _selectedCharacter = ROSS;
            _currentCameraMode = ARCBALL;
            _arcballCam->setLookAtPoint(_rossPosition);
            break;
            case GLFW_KEY_C:
                _selectedCharacter = VYRME;
            _currentCameraMode = ARCBALL;
            _arcballCam->setLookAtPoint(_vyrmePosition);
            break;
            case GLFW_KEY_1:
                    _isSmallViewportActive = !_isSmallViewportActive;
            break;
            case GLFW_KEY_2:
                _currentCameraMode = FREE_CAM;
            _freeCam->setPosition(glm::vec3(0.0f, 40.0f, 60.0f));
            _freeCam->setTheta(glm::radians(0.0f));
            _freeCam->setPhi(glm::radians(60.0f));
            _freeCam->recomputeOrientation();
            break;

            case GLFW_KEY_LEFT:
            case GLFW_KEY_RIGHT:
                if (_currentCameraMode == FREE_CAM && _isSmallViewportActive) {
                    if (key == GLFW_KEY_LEFT) {
                        // Cambiar al personaje anterior
                        _smallViewportCharacter = static_cast<Character>((_smallViewportCharacter + NUM_CHARACTERS - 1) % NUM_CHARACTERS);
                    } else if (key == GLFW_KEY_RIGHT) {
                        // Cambiar al siguiente personaje
                        _smallViewportCharacter = static_cast<Character>((_smallViewportCharacter + 1) % NUM_CHARACTERS);
                    }
                }
            break;

            default:
                break;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
            _isShiftPressed = false;
        }
    }
}





void MP::handleMouseButtonEvent(GLint button, GLint action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        _leftMouseButtonState = action;

        _isLeftMouseButtonPressed = (action == GLFW_PRESS);
        _isZooming = _isShiftPressed && _isLeftMouseButtonPressed;

        if (_isZooming) {
            _prevMousePosition = _mousePosition;
        }
    }
}


void MP::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    if (_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
        return;
    }

    if (_isLeftMouseButtonPressed) {
        float deltaX = currMousePosition.x - _mousePosition.x;
        float deltaY = currMousePosition.y - _mousePosition.y;

        int viewportWidth, viewportHeight;
        glfwGetFramebufferSize(mpWindow, &viewportWidth, &viewportHeight);

        if (_currentCameraMode == ARCBALL) {
            if (_isShiftPressed) {
                // Zoom in ArcballCam
                float sensitivity = 1.0f; // Adjust sensitivity as needed
                _arcballCam->zoom(deltaY * sensitivity);
            } else {
                // Rotate ArcballCam
                _arcballCam->rotate(deltaX, deltaY, viewportWidth, viewportHeight);
            }
        } else if (_currentCameraMode == FREE_CAM) {
            // Rotate FreeCam
            float sensitivity = 0.005f; // Adjust sensitivity as needed
            _freeCam->setTheta(_freeCam->getTheta() + deltaX * sensitivity);
            _freeCam->setPhi(_freeCam->getPhi() - deltaY * sensitivity);
            _freeCam->recomputeOrientation();
        }
    }

    _mousePosition = currMousePosition;
}



//*************************************************************************************
//
// Engine Setup

void MP::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();

    // set our callbacks
    glfwSetKeyCallback(mpWindow, A3_engine_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, A3_engine_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, A3_engine_cursor_callback);
}

void MP::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                    // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	        // clear the frame buffer to black
}

void MP::mSetupShaders() {
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/A3.v.glsl", "shaders/A3.f.glsl" );

    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.normalMatrix   = _lightingShaderProgram->getUniformLocation("normalMatrix");

    _lightingShaderUniformLocations.lightDirection     = _lightingShaderProgram->getUniformLocation("lightDirection");
    _lightingShaderUniformLocations.lightAmbientColor  = _lightingShaderProgram->getUniformLocation("lightAmbientColor");
    _lightingShaderUniformLocations.lightDiffuseColor  = _lightingShaderProgram->getUniformLocation("lightDiffuseColor");
    _lightingShaderUniformLocations.lightSpecularColor = _lightingShaderProgram->getUniformLocation("lightSpecularColor");

    _lightingShaderUniformLocations.materialAmbientColor   = _lightingShaderProgram->getUniformLocation("materialAmbientColor");
    _lightingShaderUniformLocations.materialDiffuseColor   = _lightingShaderProgram->getUniformLocation("materialDiffuseColor");
    _lightingShaderUniformLocations.materialSpecularColor  = _lightingShaderProgram->getUniformLocation("materialSpecularColor");
    _lightingShaderUniformLocations.materialShininess      = _lightingShaderProgram->getUniformLocation("materialShininess");

    _lightingShaderUniformLocations.eyePosition = _lightingShaderProgram->getUniformLocation("eyePosition");

    _lightingShaderAttributeLocations.vPos    = _lightingShaderProgram->getAttributeLocation("vPos");
    _lightingShaderAttributeLocations.vNormal = _lightingShaderProgram->getAttributeLocation("vNormal");
}


void MP::mSetupBuffers() {
    CSCI441::setVertexAttributeLocations(_lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vNormal);

    _pPlane = new Aaron_Inti( _lightingShaderProgram->getShaderProgramHandle(),
                              _lightingShaderUniformLocations.mvpMatrix,
                              _lightingShaderUniformLocations.normalMatrix);

    _rossHero = new Ross(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mvpMatrix,
                         _lightingShaderUniformLocations.normalMatrix);

    _vyrmeHero = new Vyrme(_lightingShaderProgram->getShaderProgramHandle(),
                           _lightingShaderUniformLocations.mvpMatrix,
                           _lightingShaderUniformLocations.normalMatrix);


    _createGroundBuffers();
    _generateEnvironment();
}


void MP::_createGroundBuffers() {
    // TODO #8: expand our struct
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    // TODO #9: add normal data
    Vertex groundQuad[4] = {
        { {-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
        { { 1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
        { {-1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
        { { 1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f} }
    };

    GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    // TODO #10: hook up vertex normal attribute
    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vNormal);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void MP::_generateEnvironment() {
    // Clear existing data
    _trees.clear();
    _rocks.clear();

    // Parameters for grid size and spacing
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 5.0f;
    const GLfloat GRID_SPACING_LENGTH = 5.0f;

    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f;

    srand(static_cast<unsigned int>(time(0))); // Seed RNG

    // Generate Trees
    for (GLfloat x = LEFT_END_POINT; x <= RIGHT_END_POINT; x += GRID_SPACING_WIDTH) {
        for (GLfloat z = BOTTOM_END_POINT; z <= TOP_END_POINT; z += GRID_SPACING_LENGTH) {
            if (getRand() < 0.0f) { // 30% chance to place a tree
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
                TreeData tree = { modelMatrix };
                _trees.emplace_back(tree);
            }
        }
    }

    // Generate Rocks
    for (GLfloat x = LEFT_END_POINT; x <= RIGHT_END_POINT; x += GRID_SPACING_WIDTH) {
        for (GLfloat z = BOTTOM_END_POINT; z <= TOP_END_POINT; z += GRID_SPACING_LENGTH) {
            if (getRand() < 0.0f) { // 20% chance to place a rock
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 2.0f, 0.0f, z + 2.0f));
                float scale = getRand() * 2.0f + 1.0f; // Random scale between 1 and 3
                RockData rock = { modelMatrix, scale };
                _rocks.emplace_back(rock);
            }
        }
    }
}

void MP::mSetupScene() {
    _selectedCharacter = AARON_INTI;
    _arcballCam->setCameraView(
        glm::vec3(0.0f, 50.0f, 100.0f), // Eye position
        glm::vec3(0.0f, 0.0f, 0.0f),    // Look-at point (plane position)
        CSCI441::Y_AXIS                 // Up vector
    );


    _planePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    _planeHeading = 0.0f;

    _rossPosition = glm::vec3(5.0f, -1.3f, 0.0f);
    _rossHeading = 0.0f;

    _vyrmePosition = glm::vec3(10.0f, -0.3f, 0.0f);
    _vyrmeHeading = 0.0f;

    _updateIntiFirstPersonCamera();
    _updateRossFirstPersonCamera();
    _updateVyrmeFirstPersonCamera();
    // Set up the projection matrix
    int width, height;
    glfwGetFramebufferSize(mpWindow, &width, &height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    _projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
    _cameraSpeed = glm::vec2(0.25f, 0.02f);

    // Light properties
    glm::vec3 lightDirection = glm::vec3(-1.0f, -1.0f, -1.0f);
    glm::vec3 lightAmbientColor = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);

    // Set light uniforms
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.lightDirection, lightDirection);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.lightAmbientColor, lightAmbientColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.lightDiffuseColor, lightDiffuseColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.lightSpecularColor, lightSpecularColor);

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),
    _lightingShaderUniformLocations.lightDirection,
    1,
    glm::value_ptr(lightDirection)
    );

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),
        _lightingShaderUniformLocations.lightDiffuseColor,
        1,
        glm::value_ptr(lightDiffuseColor)
    );
}

//*************************************************************************************
//
// Engine Cleanup

void MP::mCleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
}

void MP::mCleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete _pPlane;
    delete _rossHero;
    delete _vyrmeHero;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void MP::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx, glm::vec3 eyePosition) const {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.eyePosition, eyePosition);



    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundAmbientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 groundDiffuseColor = glm::vec3(0.3f, 0.8f, 0.2f); // Existing ground color
    glm::vec3 groundSpecularColor = glm::vec3(0.0f, 0.0f, 0.0f); // No specular for ground
    float groundShininess = 0.0f;

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, groundAmbientColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, groundDiffuseColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, groundSpecularColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, groundShininess);


    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////
    ///
    /////// BEGIN DRAWING THE RIVER ////
    // Draw a rectangle representing the river in the middle
    glm::mat4 riverModelMtx = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f));
    riverModelMtx = glm::scale(riverModelMtx, glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE / 5.0f));
    _computeAndSendMatrixUniforms(riverModelMtx, viewMtx, projMtx);

    glm::vec3 riverAmbientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 riverDiffuseColor = glm::vec3(0.4f, 0.8f, 1.0f); // Existing river color
    glm::vec3 riverSpecularColor = glm::vec3(0.5f, 0.5f, 0.5f);
    float riverShininess = 64.0f;

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, riverAmbientColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, riverDiffuseColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, riverSpecularColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, riverShininess);

    CSCI441::drawSolidCube(1.0f);
    //// END DRAWING THE RIVER ////

    //// BEGIN DRAWING THE TREES ////
    for (const TreeData& tree : _trees) {
        // Draw the tree trunk (brown rectangle)
        glm::mat4 modelMtx = tree.modelMatrix;
        // Trunk transformations
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 2.0f, 0.0f)); // Raise the trunk
        modelMtx = glm::scale(modelMtx, glm::vec3(0.5f, 4.0f, 0.5f)); // Scale the trunk
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);


        glm::vec3 trunkAmbientColor = glm::vec3(0.05f, 0.02f, 0.02f);
        glm::vec3 trunkDiffuseColor = glm::vec3(0.55f, 0.27f, 0.07f);
        glm::vec3 trunkSpecularColor = glm::vec3(0.1f, 0.1f, 0.1f);
        float trunkShininess = 16.0f;

        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, trunkAmbientColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, trunkDiffuseColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, trunkSpecularColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, trunkShininess);

        CSCI441::drawSolidCube(2.0f);

        // Draw the foliage (green cone)
        modelMtx = tree.modelMatrix;
        // Foliage transformations
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 5.0f, 0.0f)); // Position on top of trunk
        modelMtx = glm::scale(modelMtx, glm::vec3(2.0f, 4.0f, 2.0f)); // Scale the foliage
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
        glm::vec3 foliageAmbientColor = glm::vec3(0.0f, 0.05f, 0.0f);
        glm::vec3 foliageDiffuseColor = glm::vec3(0.0f, 0.5f, 0.0f);
        glm::vec3 foliageSpecularColor = glm::vec3(0.0f, 0.1f, 0.0f);
        float foliageShininess = 8.0f;

        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, foliageAmbientColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, foliageDiffuseColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, foliageSpecularColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, foliageShininess);
        CSCI441::drawSolidCone(0.5f, 1.0f, 16, 16);
    }
    //// END DRAWING THE TREES ////

    //// BEGIN DRAWING THE ROCKS ////
    for (const RockData& rock : _rocks) {
        glm::mat4 modelMtx = rock.modelMatrix;
        // Use the stored scale
        float scale = rock.scale;
        modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, scale / 2.0f, 0.0f)); // Raise the rock
        modelMtx = glm::scale(modelMtx, glm::vec3(scale, scale, scale)); // Scale the rock
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
        glm::vec3 rockAmbientColor = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 rockDiffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::vec3 rockSpecularColor = glm::vec3(0.3f, 0.3f, 0.3f);
        float rockShininess = 16.0f;

        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, rockAmbientColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, rockDiffuseColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, rockSpecularColor);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, rockShininess);
        CSCI441::drawSolidSphere(0.5f, 16, 16);
    }
    //// END DRAWING THE ROCKS ////

    //// BEGIN DRAWING THE CAR ////
    glm::vec3 heroAmbientColor = glm::vec3(0.1f, 0.0f, 0.0f);
    glm::vec3 heroDiffuseColor = glm::vec3(0.7f, 0.0f, 0.0f); // Red color
    glm::vec3 heroSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float heroShininess = 32.0f;

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, heroAmbientColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, heroDiffuseColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, heroSpecularColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, heroShininess);
    glm::mat4 modelMtx(1.0f);
    modelMtx = glm::translate(modelMtx, _planePosition);
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 1.3, 0.0f));
    modelMtx = glm::rotate(modelMtx, _planeHeading, CSCI441::Y_AXIS);
    _pPlane->drawVehicle(modelMtx, viewMtx, projMtx );
    //// END DRAWING THE CAR ////


    //// BEGIN DRAWING ROSS ////
    glm::vec3 RossAmbientColor = glm::vec3(0.1f, 0.0f, 0.0f);
    glm::vec3 RossDiffuseColor = glm::vec3(0.7f, 0.0f, 0.0f); // Red color
    glm::vec3 RossSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float RossShininess = 32.0f;

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, RossAmbientColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, RossDiffuseColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, RossSpecularColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, RossShininess);
    glm::mat4 modelRossMtx(1.0f);
    modelRossMtx = glm::translate(modelRossMtx, _rossPosition);
    modelRossMtx = glm::translate(modelRossMtx, glm::vec3(0.0f, 1.3, 0.0f));
    modelRossMtx = glm::rotate(modelRossMtx, _rossHeading, CSCI441::Y_AXIS);
    _rossHero->drawVehicle(modelRossMtx, viewMtx, projMtx );
    //// END DRAWING ROSS ////

    //// BEGIN DRAWING VYRME ////
    glm::vec3 VyrmeAmbientColor = glm::vec3(0.1f, 0.0f, 0.0f);
    glm::vec3 VyrmeDiffuseColor = glm::vec3(0.7f, 0.0f, 0.0f); // Red color
    glm::vec3 VyrmeSpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float VyrmeShininess = 32.0f;

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialAmbientColor, VyrmeAmbientColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialDiffuseColor, VyrmeDiffuseColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialSpecularColor, VyrmeSpecularColor);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialShininess, VyrmeShininess);
    glm::mat4 modelVyrmeMtx(1.0f);
    modelVyrmeMtx = glm::translate(modelVyrmeMtx, _vyrmePosition);
    modelVyrmeMtx = glm::translate(modelVyrmeMtx, glm::vec3(0.0f, 1.3, 0.0f));
    modelVyrmeMtx = glm::rotate(modelVyrmeMtx, _vyrmeHeading, CSCI441::Y_AXIS);
    _vyrmeHero->drawVehicle(modelVyrmeMtx, viewMtx, projMtx );
    //// END DRAWING VYRME ////
}

void MP::_updateScene() {
    float moveSpeed = 0.1f;
    float rotateSpeed = glm::radians(1.5f);

    const float MIN_X = -WORLD_SIZE + 3.0f;
    const float MAX_X = WORLD_SIZE - 3.0f;
    const float MIN_Z = -WORLD_SIZE + 3.0f;
    const float MAX_Z = WORLD_SIZE - 3.0f;

    if (_currentCameraMode == FREE_CAM) {
        // Move FreeCam with WASD
        if (_keys[GLFW_KEY_W]) {
            _freeCam->moveForward(moveSpeed);
        }
        if (_keys[GLFW_KEY_S]) {
            _freeCam->moveBackward(moveSpeed);
        }
    } else { // ARCBALL
        switch (_selectedCharacter) {
            case AARON_INTI: {
                if (_keys[GLFW_KEY_W]) {
                    glm::vec3 direction(
                        sinf(_planeHeading),
                        0.0f,
                        cosf(_planeHeading)
                    );
                    glm::vec3 newPosition = _planePosition - direction * moveSpeed;
                    newPosition.x = std::max(MIN_X, std::min(newPosition.x, MAX_X));
                    newPosition.z = std::max(MIN_Z, std::min(newPosition.z, MAX_Z));
                    _planePosition = newPosition;
                    _pPlane->moveBackward();
                }
                if (_keys[GLFW_KEY_S]) {
                    glm::vec3 direction(
                        sinf(_planeHeading),
                        0.0f,
                        cosf(_planeHeading)
                    );
                    glm::vec3 newPosition = _planePosition + direction * moveSpeed;
                    newPosition.x = std::max(MIN_X, std::min(newPosition.x, MAX_X));
                    newPosition.z = std::max(MIN_Z, std::min(newPosition.z, MAX_Z));
                    _planePosition = newPosition;
                    _pPlane->moveForward();
                }
                if (_keys[GLFW_KEY_A]) {
                    _planeHeading += rotateSpeed;
                }
                if (_keys[GLFW_KEY_D]) {
                    _planeHeading -= rotateSpeed;
                }

                if (_planeHeading > glm::two_pi<float>())
                    _planeHeading -= glm::two_pi<float>();
                else if (_planeHeading < 0.0f)
                    _planeHeading += glm::two_pi<float>();

                float Y_OFFSET = 2.0f;
                glm::vec3 intiLookAtPoint = _planePosition;
                intiLookAtPoint.y += Y_OFFSET;
                _arcballCam->setLookAtPoint(intiLookAtPoint);
                _updateIntiFirstPersonCamera();
                break;
            }

            case ROSS: {
                if (_keys[GLFW_KEY_W]) {
                    glm::vec3 direction(
                        sinf(_rossHeading),
                        0.0f,
                        cosf(_rossHeading)
                    );
                    glm::vec3 newPosition = _rossPosition - direction * moveSpeed;
                    newPosition.x = std::max(MIN_X, std::min(newPosition.x, MAX_X));
                    newPosition.z = std::max(MIN_Z, std::min(newPosition.z, MAX_Z));
                    _rossPosition = newPosition;
                    _rossHero->moveForward();
                }
                if (_keys[GLFW_KEY_S]) {
                    glm::vec3 direction(
                        sinf(_rossHeading),
                        0.0f,
                        cosf(_rossHeading)
                    );
                    glm::vec3 newPosition = _rossPosition + direction * moveSpeed;
                    newPosition.x = std::max(MIN_X, std::min(newPosition.x, MAX_X));
                    newPosition.z = std::max(MIN_Z, std::min(newPosition.z, MAX_Z));
                    _rossPosition = newPosition;
                    _rossHero->moveBackward();
                }
                if (_keys[GLFW_KEY_A]) {
                    _rossHeading += rotateSpeed;
                }
                if (_keys[GLFW_KEY_D]) {
                    _rossHeading -= rotateSpeed;
                }

                if (_rossHeading > glm::two_pi<float>())
                    _rossHeading -= glm::two_pi<float>();
                else if (_rossHeading < 0.0f)
                    _rossHeading += glm::two_pi<float>();

                float Y_OFFSET = 3.0f;
                glm::vec3 rossLookAtPoint = _rossPosition;
                rossLookAtPoint.y += Y_OFFSET;
                _arcballCam->setLookAtPoint(rossLookAtPoint);

                _updateRossFirstPersonCamera();
                break;
            }

            case VYRME: {
                if (_keys[GLFW_KEY_W]) {
                    glm::vec3 direction(
                        sinf(_vyrmeHeading),
                        0.0f,
                        cosf(_vyrmeHeading)
                    );
                    glm::vec3 newPosition = _vyrmePosition - direction * moveSpeed;
                    newPosition.x = std::max(MIN_X, std::min(newPosition.x, MAX_X));
                    newPosition.z = std::max(MIN_Z, std::min(newPosition.z, MAX_Z));
                    _vyrmePosition = newPosition;
                    _vyrmeHero->moveForward();
                }
                if (_keys[GLFW_KEY_S]) {
                    glm::vec3 direction(
                        sinf(_vyrmeHeading),
                        0.0f,
                        cosf(_vyrmeHeading)
                    );
                    glm::vec3 newPosition = _vyrmePosition + direction * moveSpeed;
                    newPosition.x = std::max(MIN_X, std::min(newPosition.x, MAX_X));
                    newPosition.z = std::max(MIN_Z, std::min(newPosition.z, MAX_Z));
                    _vyrmePosition = newPosition;
                    _vyrmeHero->moveBackward();
                }
                if (_keys[GLFW_KEY_A]) {
                    _vyrmeHeading += rotateSpeed;
                }
                if (_keys[GLFW_KEY_D]) {
                    _vyrmeHeading -= rotateSpeed;
                }

                if (_vyrmeHeading > glm::two_pi<float>())
                    _vyrmeHeading -= glm::two_pi<float>();
                else if (_vyrmeHeading < 0.0f)
                    _vyrmeHeading += glm::two_pi<float>();

                float Y_OFFSET = 2.0f;
                glm::vec3 vyrmeLookAtPoint = _vyrmePosition;
                vyrmeLookAtPoint.y += Y_OFFSET;
                _arcballCam->setLookAtPoint(vyrmeLookAtPoint);

                _updateVyrmeFirstPersonCamera();
                break;
            }

            default:
                break;
        }
    }
}

void MP::run() {
    glfwSetWindowUserPointer(mpWindow, this);

    while (!glfwWindowShouldClose(mpWindow)) {
        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(mpWindow, &framebufferWidth, &framebufferHeight);

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        float aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
        _projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

        glm::mat4 viewMatrix;
        glm::vec3 eyePosition;

        if (_currentCameraMode == ARCBALL) {
            viewMatrix = _arcballCam->getViewMatrix();
            eyePosition = _arcballCam->getPosition();
        } else if (_currentCameraMode == FREE_CAM) {
            viewMatrix = _freeCam->getViewMatrix();
            eyePosition = _freeCam->getPosition();
        } else if (_currentCameraMode == FIRST_PERSON_CAM) {
            switch (_selectedCharacter) {
                case AARON_INTI:
                    viewMatrix = _intiFirstPersonCam->getViewMatrix();
                    eyePosition = _intiFirstPersonCam->getPosition();
                    break;
                case ROSS:
                    viewMatrix = _rossFirstPersonCam->getViewMatrix();
                    eyePosition = _rossFirstPersonCam->getPosition();
                    break;
                case VYRME:
                    viewMatrix = _vyrmeFirstPersonCam->getViewMatrix();
                    eyePosition = _vyrmeFirstPersonCam->getPosition();
                    break;
                default:
                    viewMatrix = _arcballCam->getViewMatrix();
                    eyePosition = _arcballCam->getPosition();
                    break;
            }
        }

        _renderScene(viewMatrix, _projectionMatrix, eyePosition);

        if (_isSmallViewportActive) {
            GLint prevViewport[4];
            glGetIntegerv(GL_VIEWPORT, prevViewport);

            glClear(GL_DEPTH_BUFFER_BIT);

            GLint smallViewportWidth = framebufferWidth / 3;
            GLint smallViewportHeight = framebufferHeight / 3;
            GLint smallViewportX = framebufferWidth - smallViewportWidth - 10;
            GLint smallViewportY = framebufferHeight - smallViewportHeight - 10;

            glViewport(smallViewportX, smallViewportY, smallViewportWidth, smallViewportHeight);

            float smallAspectRatio = static_cast<float>(smallViewportWidth) / static_cast<float>(smallViewportHeight);

            glm::mat4 smallProjectionMatrix = glm::perspective(glm::radians(45.0f), smallAspectRatio, 0.1f, 1000.0f);

            glm::mat4 fpViewMatrix;
            glm::vec3 fpEyePosition;

            if (_currentCameraMode == FREE_CAM) {

                switch (_smallViewportCharacter) {
                    case AARON_INTI:
                        fpViewMatrix = _intiFirstPersonCam->getViewMatrix();
                        fpEyePosition = _intiFirstPersonCam->getPosition();
                        break;
                    case ROSS:
                        fpViewMatrix = _rossFirstPersonCam->getViewMatrix();
                        fpEyePosition = _rossFirstPersonCam->getPosition();
                        break;
                    case VYRME:
                        fpViewMatrix = _vyrmeFirstPersonCam->getViewMatrix();
                        fpEyePosition = _vyrmeFirstPersonCam->getPosition();
                        break;
                    default:
                        fpViewMatrix = _arcballCam->getViewMatrix();
                        fpEyePosition = _arcballCam->getPosition();
                        break;
                }
            } else {

                switch (_selectedCharacter) {
                    case AARON_INTI:
                        fpViewMatrix = _intiFirstPersonCam->getViewMatrix();
                        fpEyePosition = _intiFirstPersonCam->getPosition();
                        break;
                    case ROSS:
                        fpViewMatrix = _rossFirstPersonCam->getViewMatrix();
                        fpEyePosition = _rossFirstPersonCam->getPosition();
                        break;
                    case VYRME:
                        fpViewMatrix = _vyrmeFirstPersonCam->getViewMatrix();
                        fpEyePosition = _vyrmeFirstPersonCam->getPosition();
                        break;
                    default:
                        fpViewMatrix = _arcballCam->getViewMatrix();
                        fpEyePosition = _arcballCam->getPosition();
                        break;
                }
            }

            _renderScene(fpViewMatrix, smallProjectionMatrix, fpEyePosition);

            glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        }

        _updateScene();

        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }
}





//*************************************************************************************
//
// Private Helper FUnctions

void MP::_updateIntiFirstPersonCamera() {
    glm::vec3 offset(0.0f, 4.0f, 0.0f);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), _planeHeading, CSCI441::Y_AXIS);
    glm::vec3 rotatedOffset = glm::vec3(rotation * glm::vec4(offset, 0.0f));

    glm::vec3 cameraPosition = _planePosition + rotatedOffset;
    _intiFirstPersonCam->setPosition(cameraPosition);

    glm::vec3 facingDirection = glm::vec3(
        sinf(_planeHeading),
        0.0f,
        cosf(_planeHeading)
    );

    glm::vec3 backwardDirection = -facingDirection;
    glm::vec3 lookAtPoint = cameraPosition + backwardDirection;
    _intiFirstPersonCam->setLookAtPoint(lookAtPoint);
    _intiFirstPersonCam->computeViewMatrix();
}

void MP::_updateRossFirstPersonCamera() {
    glm::vec3 offset(0.0f, 4.0f, -0.4f);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), _rossHeading, CSCI441::Y_AXIS);
    glm::vec3 rotatedOffset = glm::vec3(rotation * glm::vec4(offset, 0.0f));

    glm::vec3 cameraPosition = _rossPosition + rotatedOffset;
    _rossFirstPersonCam->setPosition(cameraPosition);

    glm::vec3 facingDirection = glm::vec3(
        sinf(_rossHeading),
        0.0f,
        cosf(_rossHeading)
    );

    glm::vec3 backwardDirection = -facingDirection;
    glm::vec3 lookAtPoint = cameraPosition + backwardDirection;
    _rossFirstPersonCam->setLookAtPoint(lookAtPoint);
    _rossFirstPersonCam->computeViewMatrix();
}


void MP::_updateVyrmeFirstPersonCamera() {
    glm::vec3 offset(0.0f, 3.0f, -0.5f);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), _vyrmeHeading, CSCI441::Y_AXIS);
    glm::vec3 rotatedOffset = glm::vec3(rotation * glm::vec4(offset, 0.0f));

    glm::vec3 cameraPosition = _vyrmePosition + rotatedOffset;
    _vyrmeFirstPersonCam->setPosition(cameraPosition);

    glm::vec3 facingDirection = glm::vec3(
        sinf(_vyrmeHeading),
        0.0f,
        cosf(_vyrmeHeading)
    );

    glm::vec3 backwardDirection = -facingDirection;
    glm::vec3 lookAtPoint = cameraPosition + backwardDirection;
    _vyrmeFirstPersonCam->setLookAtPoint(lookAtPoint);
    _vyrmeFirstPersonCam->computeViewMatrix();
}


void MP::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

    // TODO #7: compute and send the normal matrix
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normalMatrix, normalMtx);

}

//*************************************************************************************
//
// Callbacks

void A3_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (MP*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void A3_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (MP*) glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void A3_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (MP*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
