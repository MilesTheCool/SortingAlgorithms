#include <glad/glad.h>                      // load in OpenGL functions
#include <GLFW/glfw3.h>                     // OpenGL functions
#include <iostream>                         // push stuff to terminal
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "shader.hpp"


/// @brief create the window and initialize OpenGL, returning pointer to window
/// @param width The width in pixels of the screen to be made
/// @param height The height in pixels of the screen to be made
/// @param title The title to give the created window
/// @return pointer to the created window
GLFWwindow* setupWindow(unsigned int width, unsigned int height, const char* title);

/// @brief  Function called automatically when dynamically resizing the screen
/// @param window pointer to the window object
/// @param width width in pixels of the new window
/// @param height height in pixels of the new window
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/// @brief check inputs and determine what to do as a result
/// @param window The window whose inputs are being checked
void processInput(GLFWwindow *window);

/// @brief Create and load data into the vertex array, vertex buffer, and element buffer
/// @param VBO reference to vertex buffer ID, changes the value of the var passed into it
/// @param EBO reference to element buffer ID, changes the value of the var passed into it
/// @param VAO reference to vertex array ID, changes the value of the var passed into it
void setup_buffers(unsigned int &VBO, unsigned int &EBO, unsigned int &VAO);



int main() {
    // setup opengl
    GLFWwindow* window = setupWindow(800,600,"Sorting Algorithms");

    if (window == nullptr) { 
        std::cout << "ERROR. OPENGL FAILURE" << std::endl;
        return -1;
    }

    

    unsigned int VBO, EBO, VAO;
    setup_buffers(VBO, EBO, VAO);

    // create shader
    Shader* shader = new Shader("/home/miles/dev/sorting_algorithms/src/vertex.glsl", "/home/miles/dev/sorting_algorithms/src/fragment.glsl");

    // set uniforms
    shader->use();
    shader->setMat4("trans", glm::mat4(1.0f));
    shader->setFloat("height", 1.0f);

    // height var
    double height;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render stuff

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        height = (sin(glfwGetTime()) / 2.0) + 0.5;
        shader->setFloat("height", static_cast<float>(height));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete OpenGL buffers 
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    delete shader;
    shader = nullptr;

    glfwTerminate();
    return 0;
}

// setup window and startup opengl, return refrence to window object
GLFWwindow* setupWindow(unsigned int width, unsigned int height, const char* title)
{
        // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    // close window and end program when escape is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void setup_buffers(unsigned int &VBO, unsigned int &EBO, unsigned int &VAO) {
    // create a VBO, EBO, VAO
    float vertices[] = {
        // position index
        0.0f, 0.0f, 0.0f,    // bottom left
        1.0f, 0.0f, 0.0f,    // bottom right
        0.0f, 1.0f, 0.0f,    // top left
        1.0f, 1.0f, 0.0f,    // top right
    };

    unsigned int indeces[] = {
        0, 1, 2,
        1, 2, 3,
    };

    // generate and bind Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // generate and populate Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // generate and populate the Element Buffer Object
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);

    // pass vertex info to the shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // unbind the vertex array (unbinds buffers automatically)
    // glBindVertexArray(0);

}

/* EOF */