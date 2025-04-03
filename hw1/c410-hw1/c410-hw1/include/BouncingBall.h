#ifndef BOUNCINGBALL_H
#define BOUNCINGBALL_H

#include "Angel.h"

// Object types: cube, sphere, (bonus: bunny)
enum ObjectType { CUBE, SPHERE, BUNNY };

// Global projection matrix (to be updated in the reshape callback)
extern mat4 projection;

//declare lastTime so other files can use it.
extern float lastTime;

// Function prototypes for initializing, updating, and rendering the simulation
void initBouncingBall();
void updateBouncingBall(float dt);
void renderBouncingBall();

// Callback functions for keyboard, mouse, and window resize
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

#endif
