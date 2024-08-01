#include <glad/glad.h>                      // load in OpenGL functions
#include <GLFW/glfw3.h>                     // OpenGL functions
#include <iostream>                         // push stuff to terminal
#include <glm/glm.hpp>                      // mat4s and vecs
#include <glm/gtc/matrix_transform.hpp>     // modify trans matrix
#include <vector>                           // use vector to store nums
#include <random>                           // to generate random nums
#include "shader.hpp"                       // custom shader to draw bars


/* OPENGL FUNCTIONS FOR SET-UP AND DRAWING */

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

/// @brief draw each of the numbers in the array, moving to the right each time, scaling height with value
/// @param vec a vector holding all the numers involved
/// @param shader shader object to draw with
template <class RandomIt>
void draw_array(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window);


/* RUN TIME SET-UP */

/// @brief Generate a vector of random numbers ranging from 1-max_num
/// @param size the number of nums to generate, ie size of vector being made
/// @param seed the seed for generating 'random' numbers
/// @param max_num the biggest number possible to generate
/// @return vector containing random mixed numbers
std::vector<int> random_array(size_t size, unsigned int seed, unsigned int max_num); 


/* SORTING ALGORITHMS - EACH SHOULD ONLY TAKE FIRST AND LAST ITERATORS TO SORT  (plus opengl shader to draw) */
/// @brief sort a vector or other data type that can be traversed with iterators
/// from least to biggest, assuming they are full of numbers
/// at each step, if graphics are enabled, then when a number is overwritten it will be displayed
/// std::is_sorted() will return true after each one

template <class RandomIt>
void bubble_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window);


bool sort = false;
const int MAX_NUM = 1000;
const int SIZE = 1000;

int main() {
    // setup opengl
    GLFWwindow* window = setupWindow(1000,1000,"Sorting Algorithms");

    if (window == nullptr) { 
        std::cout << "ERROR. OPENGL FAILURE" << std::endl;
        return -1;
    }

    
    
    std::random_device rd;
    const unsigned int SEED = rd();

    std::vector<int> vec = random_array(SIZE, SEED, MAX_NUM);

    // set up all vars needed for drawing

    const glm::mat4 perspective = glm::ortho(-1.0f, static_cast<float>(SIZE + 1), -1.0f, static_cast<float>(MAX_NUM + 1));

    unsigned int VBO, EBO, VAO;
    setup_buffers(VBO, EBO, VAO);

    // create shader
    Shader* shader = new Shader("/home/miles/dev/sorting_algorithms/src/vertex.glsl", "/home/miles/dev/sorting_algorithms/src/fragment.glsl");

    // set uniforms
    shader->use();
    shader->setMat4("perspective", perspective);

    

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        draw_array(vec.begin(), vec.end(), shader, window);       

        if (sort) {
            bubble_sort(vec.begin(), vec.end(), shader, window);
            sort = false;
        } 
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

/* OPEN GL FUNCTIONS FOR DRAWING */
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        sort = true;
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

template <class RandomIt>
void draw_array(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window){
    // render stuff
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
    int index = 0;
    for (auto i = first; i < last; ++i){
        // height is num/max 
        float height = static_cast<float>(*i) / static_cast<float>(MAX_NUM);   // change to floats to avoid integer division
        shader->setFloat("height", height);

        // transformation, move x to i, scale y to nums[i]
        glm::mat4 trans{1.0f};
        trans = glm::translate(trans, glm::vec3(static_cast<float>(index), 0.0f, 0.0f));
        trans = glm::scale(trans, glm::vec3(1.0f, static_cast<float>(*i), 1.0f));
        shader->setMat4("trans", trans);

        // draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        ++index;  // used to move next rect to the right
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

/* SET UP RUN-TIME DETAILS */
std::vector<int> random_array(size_t size, unsigned int seed, unsigned int max_num) {

    // create a random number generator
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distrib(1, max_num);

    // preallocate vector with max size needed
    std::vector<int> vec(size);

    // fill vector with nums
    for (int& value : vec) {
        value = distrib(gen);
    }

    return vec;
}

/* SORTING ALGORITHMS - EACH SHOULD ONLY TAKE FIRST AND LAST ITERATORS TO SORT */

template <class RandomIt>
void bubble_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window) {

    bool swapped = true;  // end early if no swaps made

    // iterale n-1 times through array
    for (auto i = first; i < last && swapped; ++i){
        swapped = false;

        // iterate through each element in array (except the last)
        for (auto current = first; current < last - 1 - (i - first); ++current){

            // compare adjacent cells, swap if out of order
            if (*current > *(current + 1)) {
                std::swap(*current, *(current + 1));
                swapped = true;
            }
        }
        
        // draw after at least 1 number is in final resting place
        draw_array(first, last, shader, window);
    }
}

/* EOF */