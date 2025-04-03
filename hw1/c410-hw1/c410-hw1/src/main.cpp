#include "Angel.h"
#include "BouncingBall.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "ERROR: GLFW initialization failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Set OpenGL version (3.3 Core) and forward compatibility (for macOS)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(800, 800, "Bouncing Ball Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "ERROR: Window creation failed" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    
    // For macOS, GLEW is not required; otherwise, initialize GLEW here.
#if !defined(__APPLE__)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: GLEW initialization failed" << std::endl;
        exit(EXIT_FAILURE);
    }
#endif

    // Set callback functions
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Initialize the bouncing ball simulation
    initBouncingBall();
    
    // Set initial projection matrix based on current window size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / height;
    if (aspect >= 1.0)
        projection = Ortho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    else
        projection = Ortho(-1.0, 1.0, -1.0/aspect, 1.0/aspect, -1.0, 1.0);
    
    lastTime = (float)glfwGetTime();
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        
        updateBouncingBall(dt);
        renderBouncingBall();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
