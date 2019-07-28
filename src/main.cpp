#include "utils/shader.h"

#include <GLUT/glut.h>
#include <GLFW/glfw3.h>

#include "utils/stb_image.h"

#include <iostream>
#include <cmath>

// Import OpenGL Mathematics
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

// Declare functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float processPlayer1Input(GLFWwindow *window);
float processPlayer2Input(GLFWwindow *window);

int main()
{
    // Initialize GLFW and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // safe on mac

    // Create a window object
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);
    // For resizing the window > set the callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    // Initialise GLEW
    if(GLEW_OK != glewInit()){
      std::cout << "Failed to initialize GLEW" << std::endl;

      return -1;
    }

    // Create a shader object
    Shader ourShader("/Users/simonbelanger/Documents/Personnel/Code/cpp/learnOpenGL/bin/Shaders/vShader.glsl", 
    "/Users/simonbelanger/Documents/Personnel/Code/cpp/learnOpenGL/bin/Shaders/fShader.glsl");

    // Rectangle to render in Normalized Device Coordinates (NDC)
    float vertices[] = {
    // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    // Buffer
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); // Vertex array
    glGenBuffers(1, &VBO);      // Vertex Array Buffer
    glGenBuffers(1, &EBO);      // Element Array Buffer

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Linking position vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8  * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Linking color vertex attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // Linking texture vertex attributes
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load a texture
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("/Users/simonbelanger/Documents/Personnel/Code/cpp/learnOpenGL/bin/Textures/container.jpg", 
                          &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //load and generate another texture
    stbi_set_flip_vertically_on_load(true);  
    data = stbi_load("/Users/simonbelanger/Documents/Personnel/Code/cpp/learnOpenGL/bin/Textures/awesomeface.png", 
                          &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use(); // don't forget to activate the shader before setting uniforms!  
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually
    ourShader.setInt("texture2", 1); // or with shader class

    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    // Game logic parameters
    float objectVelocityX  = 1.0f;           // X Velocity of the object (contains direction as the sign)
    float objectVelocityY  = 0.5f;           // Y Velocity of the object (contains direction as the sign)
    float initialPositionX = 0.0f;             // X Initial position of the object
    float initialPositionY = 0.0f;             // Y Initial position of the object
    float objectPositionX  = initialPositionX;  // Initialize object X position to initial X position
    float objectPositionY  = initialPositionY;  // Initialize object Y position to initial Y position

    // Paddles
    float player1Velocity = 0.0f;
    float player2Velocity = 0.0f;
    float player1Initialpos = 0.0f;
    float player2Initialpos = 0.0f;
    float player1Position = player1Initialpos;
    float player2Position = player1Initialpos;

    // Render loop
    while(!glfwWindowShouldClose(window)){

      // Calculate delta time
      GLfloat currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      // Input
      processInput(window);

      // Rendering the background 
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // bind texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);

      // Check if the object is at the left/right boundary. If yes reverse direction.
      if(abs(objectPositionX) >= 1.0f){
        objectVelocityX = -1.0f * objectVelocityX;
      }
      // Check if the object is at the top/bottom boundary. If yes reverse direction.
      if(abs(objectPositionY) >= 1.0f){
        objectVelocityY = -1.0f * objectVelocityY;
      }

      // Increment object position and translate
      objectPositionX = objectPositionX + objectVelocityX * deltaTime;
      objectPositionY = objectPositionY + objectVelocityY * deltaTime;
      glm::mat4 trans = glm::mat4(1.0f);
      trans = glm::translate(trans, glm::vec3(objectPositionX, objectPositionY, 0.0f));
      trans = glm::scale(trans, glm::vec3(0.1f, 0.1f, 0.0f));    

      // Apply the translation vec to the shader's uniform                     
      unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

      // Render the ball
      ourShader.use();
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      // Left Paddle position
      player1Velocity = processPlayer1Input(window);
      player1Position = player1Position + player1Velocity * deltaTime;
      trans = glm::mat4(1.0f);
      trans = glm::translate(trans, glm::vec3(-0.95f, player1Position, 0.0f));
      trans = glm::scale(trans, glm::vec3(0.05f, 0.5f, 0.0f));    

      // Apply the translation vec to the shader's uniform                     
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

      // Render the paddle
      ourShader.use();
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      // Right Paddle position
      player2Velocity = processPlayer2Input(window);
      player2Position = player2Position + player2Velocity * deltaTime;
      trans = glm::mat4(1.0f);
      trans = glm::translate(trans, glm::vec3(0.95f, player2Position, 0.0f));
      trans = glm::scale(trans, glm::vec3(0.05f, 0.5f, 0.0f));    

      // Apply the translation vec to the shader's uniform                     
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

      // Render the paddle
      ourShader.use();
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      // Check/Call events and swap the buffers
      glfwSwapBuffers(window);   
      glfwPollEvents(); 
    }

    // Clean GLFW resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
  
    return 0;
}

// Function to execute when resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// Function to check if the escape key has been pressed
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Process Inputs player
float processPlayer1Input(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        return 2.0f;
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        return -2.0f;
    else
        return 0.0f;
}

// Process Inputs player
float processPlayer2Input(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        return 2.0f;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        return -2.0f;
    else
        return 0.0f;
}