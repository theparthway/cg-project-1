// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "meshObject.hpp"
#include "gridObject.hpp"
#include "projectile.hpp"

// Function prototypes
int initWindow(void);
static void mouseCallback(GLFWwindow*, int, int, int);
int getPickedId();


const GLuint windowWidth = 1024, windowHeight = 768;
GLFWwindow* window;

// The object that is currently selected for P1bTask4
// 0 camera
// 1 base translation
// 2 base rotation
// 3 arm1 rotation
// 4 arm2 rotation
int currSelected = 0;
bool rotation = false;

// bonus
Projectile* projectile = nullptr;
bool teleportInProgress = false;
bool teleportCompleted = false;
float teleportTimer = 0.0f;
const float teleportDuration = 1.0f; // Time to complete teleport animation


int main() {
    // ATTN: REFER TO https://learnopengl.com/Getting-started/Creating-a-window
    // AND https://learnopengl.com/Getting-started/Hello-Window to familiarize yourself with the initialization of a window in OpenGL
    
    if (initWindow() != 0) return -1;

    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 projectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Or, for Project 2, use an ortho camera :
    // gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

    
    //TODO: P1aTask3 - Create each robot arm piece by creating instances class meshObject.
    gridObject grid;
    meshObject base("../Base.obj");
    meshObject arm1("../Arm1.obj");
    meshObject joint("../joint.obj");
    meshObject arm2("../arm2.obj");
    
    // TODO: P1bTask4 - Create a hierarchical structure and adjust the relative translations.
    base.addChild(&arm1);
    arm1.addChild(&joint);
    joint.addChild(&arm2);
    
    // TODO: P1aTask3 - Translate each robot arm piece to its approapriate location
    base.translate(glm::vec3(0,0,0));
    arm1.translate(glm::vec3(0,0.5,0));
    arm1.rotate(-30.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    joint.translate(glm::vec3(0,4,0));
    arm2.translate(glm::vec3(0,0,0));
    arm2.rotate(-120.0f, glm::vec3(1.0f, 0.0f, 0.0f));


    //TODO: P1aTask2 - Create variables to keep track of camera angles.
    float cameraRadius = 15.0f;
    float theta = 45.0f;
    float phi = 45.0f;
    float cameraSpeed = 60.0f;
    bool isUpsideDown = false;
    
    //TODO: P1bTask5 - Create variables to store lighting info.
    // Lighting variables
    glm::vec3 light1Position = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 light1Ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 light1Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 light1Specular = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 light2Position = glm::vec3(5.0f, 5.0f, -5.0f);
    glm::vec3 light2Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 light2Diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
    glm::vec3 light2Specular = glm::vec3(1.0f, 1.0f, 1.0f);

    // Material properties
    glm::vec3 materialAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 materialDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 materialSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    float materialShininess = 32.0f;

    LightingInfo lightingInfo;
    
    double lastFrameTime = glfwGetTime();
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do {
        
        // Timing
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        
        double frameTime = glfwGetTime();
        float deltaTime = float(frameTime - lastFrameTime);
        lastFrameTime = frameTime;
        //TODO: P1aTask2 - set currSelected to 0 when key c is pressed.
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            currSelected = 0;
        }
        
        if (currSelected == 0) {
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                //TODO: P1aTask2 - adjust the camera rotation.
                // Note: to make adjustments independent of frame rate, use time since last frame to make adjustment.
                theta += cameraSpeed * deltaTime;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                //TODO: P1aTask2 - adjust the camera rotation.
                theta -= cameraSpeed * deltaTime;
            }
    
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                phi += cameraSpeed * deltaTime;
                if (phi > 180.0f) {
                    phi -= 360.0f;
                    isUpsideDown = !isUpsideDown;
                }
            }
    
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                phi -= cameraSpeed * deltaTime;
                if (phi < -180.0f) {
                    phi += 360.0f;
                    isUpsideDown = !isUpsideDown;
                }
            }
        }
        
        
        //TODO: P1aTask2 - Add the cases for movment along the other axis.
        //TODO: P1aTask2 - Create the view matrix based on camera angles.
        float x = cameraRadius * cos(glm::radians(theta)) * sin(glm::radians(phi));
        float y = cameraRadius * cos(glm::radians(phi));
        float z = cameraRadius * sin(glm::radians(theta)) * sin(glm::radians(phi));
        glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(x, y, z),   // Camera position
            glm::vec3(0.0f),  // Look at the origin
            isUpsideDown ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0)  // Head is looking up at the origin (set to 0,-1,0 to look upside-down)
        );

        // Update light positions relative to the camera
        glm::vec3 cameraPosition = glm::vec3(x, y, z);
        glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0, 1, 0), glm::normalize(cameraPosition)));
        glm::vec3 cameraUp = glm::normalize(glm::cross(glm::normalize(cameraPosition), cameraRight));

        light1Position = cameraPosition + (cameraRight * -3.0f) + (cameraUp * 2.0f);
        light2Position = cameraPosition + (cameraRight * 3.0f) + (cameraUp * 2.0f);

        // Material properties
        lightingInfo.materialAmbient = materialAmbient;
        lightingInfo.materialDiffuse = materialDiffuse;
        lightingInfo.materialSpecular = materialSpecular;
        lightingInfo.materialShininess = materialShininess;
        
        // Draw picking for P1bBonus2
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //TODO: P1bBonus2 draw all robort arm pieces using drawPicking function
            base.drawPicking(viewMatrix, projectionMatrix);
            currSelected = getPickedId();
            
            std::cout << "Picked id: " << currSelected << std::endl;
            
            //TODO: P1bBonus2 - meshObject::getMeshObjectById can be used to get the picked object.
        }
        
        //TODO: P1bTask4 - On key press set currSelected to the id of the robot piece to select.
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            currSelected = base.getId();
            rotation = false;
            std::cout << "base translation selected" << std::endl;
        } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            currSelected = base.getId();
            rotation = true;
            std::cout << "base rotation selected" << std::endl;
        } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            currSelected = arm1.getId();
            std::cout << "arm1 rotation selected" << std:: endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            currSelected = arm2.getId();
            std::cout << "arm2 rotation selected" << std:: endl;
        }

        // bonus 1
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !teleportInProgress && !projectile) {
            // Calculate tip of Arm2 position and direction
            glm::mat4 baseModelMatrix = base.modelMatrix;
            glm::mat4 arm1ModelMatrix = baseModelMatrix * arm1.modelMatrix;
            glm::mat4 jointModelMatrix = arm1ModelMatrix * joint.modelMatrix;
            glm::mat4 arm2ModelMatrix = jointModelMatrix * arm2.modelMatrix;
            
            // Calculate the forward direction of Arm2 (assuming Z-axis is forward)
            glm::vec3 arm2Direction = glm::normalize(glm::vec3(
                arm2ModelMatrix[2][0],
                arm2ModelMatrix[2][1],
                arm2ModelMatrix[2][2]
            ));
            
            // Calculate the tip position (assuming the tip is at the end of Arm2)
            glm::vec3 arm2Position = glm::vec3(arm2ModelMatrix[3]);
            float arm2Length = 4.0f; // Adjust based on your model
            glm::vec3 tipPosition = arm2Position + arm2Direction * arm2Length;
            
            // Initial velocity based on Arm2 direction and length
            glm::vec3 initialVelocity = arm2Direction * arm2Length * 2.0f;
            initialVelocity.y = std::max(initialVelocity.y, 5.0f); // Ensure some upward velocity
            
            // Create tetrahedron projectile (or any other Platonic solid you have)
            glm::vec3 testPosition = glm::vec3(0.0f, 5.0f, 0.0f); // Directly above the scene
            glm::vec3 testVelocity = glm::vec3(0.0f, 0.0f, 0.0f); // No initial velocity
            projectile = new Projectile("../cube.obj", tipPosition, initialVelocity);
            teleportInProgress = true;
        }

        // Update and draw projectile
        if (projectile) {
            projectile->update(deltaTime);
            projectile->draw(viewMatrix, projectionMatrix, lightingInfo);
            
            // Check if projectile hit ground
            if (projectile->hasHitGround() && !teleportCompleted) {
                teleportTimer += deltaTime;

                std::cout << "teleport complete" << std::endl;
                
                if (teleportTimer >= teleportDuration) {
                    // Teleport the robot to impact location
                    glm::vec3 impactPosition = projectile->getPosition();
                    
                    // Reset base position and apply new position
                    base.modelMatrix = glm::mat4(1.0f);
                    base.translate(glm::vec3(impactPosition.x, 0.0f, impactPosition.z));
                    
                    // Clean up
                    delete projectile;
                    projectile = nullptr;
                    teleportInProgress = false;
                    teleportCompleted = false;
                    teleportTimer = 0.0f;
                }
            }
        }
        
        //TODO: P1bTask4 - On key press, based on currSelected, make appropriate transformation.
        // base
        if (currSelected == base.getId()) {
            if (rotation == false) {
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    base.translate(glm::vec3(-2.0f * deltaTime, 0.0f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    base.translate(glm::vec3(2.0f * deltaTime, 0.0f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                    base.translate(glm::vec3(0.0f, 0.0f, -2.0f * deltaTime));
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    base.translate(glm::vec3(0.0f, 0.0f, 2.0f * deltaTime));
                }
            } else {
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                    base.rotate(45.0f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                    base.rotate(-45.0f * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
        }

        // arm1 rotation
        if (currSelected == arm1.getId()) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                arm1.rotate(45.0f * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                arm1.rotate(-45.0f * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }

        // arm2 rotation
        if (currSelected == arm2.getId()) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                arm2.rotate(45.0f * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                arm2.rotate(-45.0f * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // DRAWING the SCENE
        
        //TODO: P1aTask3 - Draw all robot arm pieces.
        // grid.draw(viewMatrix, projectionMatrix);
        // base.draw(viewMatrix, projectionMatrix);
        // arm1.draw(viewMatrix, projectionMatrix);
        // joint.draw(viewMatrix, projectionMatrix);
        // arm2.draw(viewMatrix, projectionMatrix);
        
        //TODO: P1bTask4 - Draw the robot arm pieces using the hierachy instead. Call the draw function on the root node. The remeaining pieces will be drawn using recursive calls.
        grid.draw(viewMatrix, projectionMatrix);

        // Create lighting info struct and pass it to the draw function
        LightingInfo lightingInfo;
        lightingInfo.cameraPosition = cameraPosition;

        lightingInfo.light1Position = light1Position;
        lightingInfo.light1Ambient = light1Ambient;
        lightingInfo.light1Diffuse = light1Diffuse;
        lightingInfo.light1Specular = light1Specular;

        lightingInfo.light2Position = light2Position;
        lightingInfo.light2Ambient = light2Ambient;
        lightingInfo.light2Diffuse = light2Diffuse;
        lightingInfo.light2Specular = light2Specular;

        lightingInfo.materialAmbient = materialAmbient;
        lightingInfo.materialDiffuse = materialDiffuse;
        lightingInfo.materialSpecular = materialSpecular;
        lightingInfo.materialShininess = materialShininess;

        base.drawWithChildren(viewMatrix, projectionMatrix, glm::mat4(1.0f), currSelected, lightingInfo);

        //TODO: P1bTask5 - Pass the lighting info to the draw function.
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

    glfwTerminate();
    return 0;
}

// Initialize GLFW and create a window
int initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC

    window = glfwCreateWindow(windowWidth, windowHeight, "Shah,Parth(90226836)", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Set up inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    return 0;
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Left mouse button pressed" << std::endl;
    }
}

int getPickedId(){
    glFlush();
    // --- Wait until all the pending drawing commands are really done.
    // Ultra-mega-over slow !
    // There are usually a long time between glDrawElements() and
    // all the fragments completely rasterized.
    glFinish();
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned char data[4];

    //TODO: P1bBonus2 - Use glfwGetCursorPos to get the x and y value of the cursor.
    
    //TODO: P1bBonus2 - Use glfwGetFramebufferSize and glfwGetWindowSize to get the frame buffer size and window size. On high resolution displays, these sizes might be different.
    
    
    //TODO: P1bBonus2 - Use glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data) to read the pixel data.
    // Note that y position has to be flipped as glfwGetCursorPos gives the cursor position relative to top left of the screen. The read location must also be multiplied by (buffer size / windowSize) for some displays.
    
    int pickedId = data[0];
    return pickedId;
}
