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
                                _isZooming(false){

    for(auto& _key : _keys) _key = GL_FALSE;
    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;
    _arcballCam = new ArcballCam();
}

MP::~MP() {
    delete _arcballCam;
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

    if (_isZooming) {

        float deltaY = currMousePosition.y - _prevMousePosition.y;
        _arcballCam->zoom(deltaY * 1.0f);


        _prevMousePosition = currMousePosition;
    }
    else if (_leftMouseButtonState == GLFW_PRESS && !_isShiftPressed) {
        float deltaX = currMousePosition.x - _mousePosition.x;
        float deltaY = currMousePosition.y - _mousePosition.y;

        int viewportWidth, viewportHeight;
        glfwGetFramebufferSize(mpWindow, &viewportWidth, &viewportHeight);

        _arcballCam->rotate(deltaX, deltaY, viewportWidth, viewportHeight);
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
            if (getRand() < 0.3f) { // 30% chance to place a tree
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
                TreeData tree = { modelMatrix };
                _trees.emplace_back(tree);
            }
        }
    }

    // Generate Rocks
    for (GLfloat x = LEFT_END_POINT; x <= RIGHT_END_POINT; x += GRID_SPACING_WIDTH) {
        for (GLfloat z = BOTTOM_END_POINT; z <= TOP_END_POINT; z += GRID_SPACING_LENGTH) {
            if (getRand() < 0.2f) { // 20% chance to place a rock
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 2.0f, 0.0f, z + 2.0f));
                float scale = getRand() * 2.0f + 1.0f; // Random scale between 1 and 3
                RockData rock = { modelMatrix, scale };
                _rocks.emplace_back(rock);
            }
        }
    }
}

void MP::mSetupScene() {
    _arcballCam->setCameraView(
        glm::vec3(0.0f, 50.0f, 100.0f), // Eye position
        glm::vec3(0.0f, 0.0f, 0.0f),    // Look-at point (plane position)
        CSCI441::Y_AXIS                 // Up vector
    );
    _planePosition = glm::vec3(0.0f, 0.0f, 0.0f);
    _planeHeading = 0.0f;

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
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void MP::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    glm::vec3 eyePosition = _arcballCam->getPosition();
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
}

void MP::_updateScene() {
    float moveSpeed = 0.08f;
    float rotateSpeed = glm::radians(1.5f);

    const float MIN_X = -WORLD_SIZE+3;
    const float MAX_X = WORLD_SIZE-3;
    const float MIN_Z = -WORLD_SIZE+3;
    const float MAX_Z = WORLD_SIZE-3;

    glm::vec3 direction(0.0f);

    // Car movement
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
    if (_keys[GLFW_KEY_W]) {
        // Move backward along heading
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
    if (_keys[GLFW_KEY_A]) {
        // Turn left
        _planeHeading += rotateSpeed;
    }
    if (_keys[GLFW_KEY_D]) {
        // Turn right
        _planeHeading -= rotateSpeed;
    }

    // Keep heading
    if (_planeHeading > glm::two_pi<float>())
        _planeHeading -= glm::two_pi<float>();
    else if (_planeHeading < 0.0f)
        _planeHeading += glm::two_pi<float>();

    // Update camera to center around car
    _arcballCam->setLookAtPoint(_planePosition);

}

void MP::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.

    glfwSetKeyCallback(mpWindow, A3_engine_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, A3_engine_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, A3_engine_cursor_callback);

    while( !glfwWindowShouldClose(mpWindow) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );
        float aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
        _projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

        // draw everything to the window
        _renderScene(_arcballCam->getViewMatrix(), _projectionMatrix);

        _updateScene();

        glfwSwapBuffers(mpWindow);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper FUnctions

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
