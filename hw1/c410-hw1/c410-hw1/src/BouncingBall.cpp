#include "BouncingBall.h"
#include <vector>
#include <cmath>

// Global state variables
ObjectType currentObject = CUBE;
bool wireframeMode = false;
vec3 currentColor(1.0, 0.0, 0.0); // Start with red; toggle to blue

// Ball physics variables (using normalized coordinates)
vec2 ballPos(-0.9, 0.9);    // Start near top-left
vec2 ballVel(0.5, 0.0);     // Initial horizontal velocity
const float gravity = -1.0;
const float bounceDamping = 0.8;

float lastTime = 0.0;

// Shader program and geometry IDs
GLuint shaderProgram;
GLuint cubeVAO, cubeVBO;
GLuint sphereVAO, sphereVBO, sphereEBO;
unsigned int sphereIndexCount = 0;

// Global projection matrix (defined as a mat4 using Angel's mat.h)
mat4 projection;

// Forward declarations of helper functions
void printHelp();
void initCube();
void initSphere();

void printHelp() {
    std::cout << "Controls:\n"
              << "  Mouse Left Button: Toggle drawing mode (wireframe/solid)\n"
              << "  Mouse Right Button: Toggle object type (cube/sphere)\n"
              << "  Keyboard 'i': Reinitialize ball position\n"
              << "  Keyboard 'c': Change color (red <-> blue)\n"
              << "  Keyboard 'h': Print help\n"
              << "  Keyboard 'q': Quit program\n";
}

void initCube() {
    // Define cube vertices (size 0.2) centered at origin
    GLfloat vertices[] = {
        -0.1, -0.1,  0.1,
         0.1, -0.1,  0.1,
         0.1,  0.1,  0.1,
        -0.1,  0.1,  0.1,
        -0.1, -0.1, -0.1,
         0.1, -0.1, -0.1,
         0.1,  0.1, -0.1,
        -0.1,  0.1, -0.1,
    };
    GLuint indices[] = {
        // front face
        0, 1, 2, 2, 3, 0,
        // right face
        1, 5, 6, 6, 2, 1,
        // back face
        7, 6, 5, 5, 4, 7,
        // left face
        4, 0, 3, 3, 7, 4,
        // bottom face
        4, 5, 1, 1, 0, 4,
        // top face
        3, 2, 6, 6, 7, 3
    };
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

void initSphere() {
    const unsigned int X_SEGMENTS = 20;
    const unsigned int Y_SEGMENTS = 20;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    float radius = 0.1;
    
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / X_SEGMENTS;
            float ySegment = (float)y / Y_SEGMENTS;
            float xPos = radius * std::cos(xSegment * 2.0 * M_PI) * std::sin(ySegment * M_PI);
            float yPos = radius * std::cos(ySegment * M_PI);
            float zPos = radius * std::sin(xSegment * 2.0 * M_PI) * std::sin(ySegment * M_PI);
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }
    
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
    }
    
    sphereIndexCount = indices.size();
    
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    
    GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

void initBouncingBall() {
    // Load shaders – paths here are relative to the project directory
    shaderProgram = Angel::InitShader("shaders/vshader.glsl", "shaders/fshader.glsl");
    glUseProgram(shaderProgram);
    
    // Initialize geometry
    initCube();
    initSphere();
    
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glEnable(GL_DEPTH_TEST);
    
    printHelp();
    lastTime = (float)glfwGetTime();
}

void updateBouncingBall(float dt) {
    // Update vertical velocity with gravity
    ballVel.y += gravity * dt;
    ballPos = ballPos + ballVel * dt;
    
    // Bounce off the bottom (assume view bottom is at y = -1.0)
    if (ballPos.y - 0.1 <= -1.0) {
        ballPos.y = -1.0 + 0.1;
        ballVel.y = -ballVel.y * bounceDamping;
        if (std::fabs(ballVel.y) < 0.01)
            ballVel.y = 0.0;
    }
    
    // Stop horizontal movement if ball reaches right edge (x = 1.0)
    /*if (ballPos.x + 0.1 >= 1.0)
        ballVel.x = 0.0;*/
    //don't want this rn.
}

void renderBouncingBall() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    
    // Send the projection matrix uniform; using Angel's mat.h functions
    GLuint projLoc = glGetUniformLocation(shaderProgram, "Projection");
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, projection);
    
    // Send object color uniform
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLfloat color[3] = { (GLfloat)currentColor.x, (GLfloat)currentColor.y, (GLfloat)currentColor.z };
    glUniform3fv(colorLoc, 1, color);
    
    // Create model matrix: translate by ballPos
    mat4 model = Translate(ballPos.x, ballPos.y, 0.0);
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "ModelView");
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model);
    
    // Draw the selected object
    if (currentObject == CUBE) {
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    } else if (currentObject == SPHERE) {
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLE_STRIP, sphereIndexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

// Callback: Keyboard input to reset position, change color, print help, or quit.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_I:
                ballPos = vec2(-0.9, 0.9);
                ballVel = vec2(0.5, 0.0);
                break;
            case GLFW_KEY_C:
                currentColor = (currentColor == vec3(1.0, 0.0, 0.0)) ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
                break;
            case GLFW_KEY_H:
                printHelp();
                break;
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            default:
                break;
        }
    }
}

// Callback: Mouse button toggles drawing mode and object type.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            wireframeMode = !wireframeMode;
            glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            currentObject = (currentObject == CUBE) ? SPHERE : CUBE;
        }
    }
}

// Callback: When the window is resized, update the viewport and projection matrix.
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    float aspect = (float)width / (float)height;
    if (aspect >= 1.0)
        projection = Ortho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    else
        projection = Ortho(-1.0, 1.0, -1.0/aspect, 1.0/aspect, -1.0, 1.0);
}
