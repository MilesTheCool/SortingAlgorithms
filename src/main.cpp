#include "glad.h"                      // load in OpenGL functions
#include <GLFW/glfw3.h>                     // OpenGL functions
#include <iostream>                         // push stuff to terminal
#include <glm/glm.hpp>                      // mat4s and vecs
#include <glm/gtc/matrix_transform.hpp>     // modify trans matrix
#include <vector>                           // use vector to store nums
#include <random>                           // to generate random nums
#include "shader.hpp"                       // custom shader to draw bars
#include <algorithm>
#include <unistd.h>                         // used to import sleep() function
#include <chrono>                           // benchmark function
#include <cstdint>                          // benchmark function


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

/// @brief change the perspective and vector for a new size - some algorithms take too long on big arrays
/// @param new_size  - the number of elements to put in the array, will be modifed
/// @param shader - shader to change the mat4 for perspective
/// @return the new vector of nums 1 - size
std::vector<int> change_size(const int new_size, Shader* shader);


/* SORTING ALGORITHMS - EACH SHOULD ONLY TAKE FIRST AND LAST ITERATORS TO SORT  (plus opengl shader to draw) */
/// @brief sort a vector or other data type that can be traversed with iterators
/// from least to biggest, assuming they are full of numbers
/// at each step, if graphics are enabled, then when a number is overwritten it will be displayed
/// std::is_sorted() will return true after each one

// traverse list, comparing adjacent items and moving the larger one towards the end
template <class RandomIt>
void bubble_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window);

// similar to bubble, but when the end is reached traverse backwards and move the smaller items to the beginning
template <class RandomIt>
void shaker_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window);

// search the list and find the smallest element, swap it to the beginning of the unsorted portion, and update the sorted portion to include it
template <class RandomIt>
void selection_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window);

// sorted and unsorted portion of the array. 
// insert the first unsorted num into the sorted portion, shuffling numbers as needed - poor performance on arrays due to insert heavy algorithm
template <class RandomIt>
void insertion_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window);


// due to recursive implementation, the starting first and last pointers need to be kept track of for drawing. last 4 arguments can be
// removed from every call for a regular sorting array
template <typename RandomIt>
void quicksort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window, RandomIt OG_first, RandomIt OG_last);



/**
 *  @brief Measures the execution time of a function in nanoseconds.
 *  @note COPIED DIRECTLY FROM CANVAS, NOT MY WORK
 *
 *  @details This function template takes a callable and its arguments, executes
 *  the callable with the provided arguments, and measures the execution time
 *  using high-resolution clock. It returns the duration taken for the function
 *  call to execute in nanoseconds.
 *
 *  This can be used to benchmark algorithms, function calls, or any callable
 *  constructs. The function being benchmarked is called exactly once. The
 *  precision and accuracy of the benchmark depend on the resolution of the
 *  clock on the system where the code is run.
 *
 *  Example usage:
 *  @code
 *  auto result = benchmark([](int x){ return x * x; }, 10);
 *  std::cout << "Time taken: " << result << " nanoseconds" << std::endl;
 *  @endcode
 *
 *  @tparam Func The type of the callable to be benchmarked.
 *  @tparam Args The types of the arguments to pass to the callable.
 *  @param function A reference to the callable to be benchmarked.
 *  @param args The arguments to pass to the callable.
 *  @return The execution time of the callable in nanoseconds as an int64_t.
 */

template <class Func, class... Args>
int64_t benchmark(const Func& function, Args... args) {
    using namespace std::chrono;

    const auto start = high_resolution_clock::now();
    function(args...);
    const auto stop  = high_resolution_clock::now();

    return duration_cast<nanoseconds>(stop - start).count();
}


int size = 50;

int main() {
    // setup opengl
    GLFWwindow* window = setupWindow(500,500,"Sorting Algorithms");

    if (window == nullptr) { 
        std::cout << "ERROR. OPENGL FAILURE" << std::endl;
        return -1;
    }


    unsigned int VBO, EBO, VAO;
    setup_buffers(VBO, EBO, VAO);

    // create shader
    Shader* shader = new Shader("/home/miles/dev/sorting_algorithms/src/vertex.glsl", "/home/miles/dev/sorting_algorithms/src/fragment.glsl");
    shader->use();
    
    std::vector<int> vec = change_size(size, shader);

    // render loop
    for (int i = 0; i < 6 && !glfwWindowShouldClose(window); ++i)
    {
        // input
        processInput(window);
        if (glfwWindowShouldClose(window)) {break;}

        int target_size;    // change the size of vec over time to display change to user
        double seconds;     // time for benchmarking functions

        switch (i) {
            case 0:   // display start countdown
                std::cout << "staring in x" << std::flush;

                for (int j = 3; j > 0; --j){
                    std::cout << "\b" << j << std::flush;
                    processInput(window);
                    if (glfwWindowShouldClose(window)) {std::cout << "\nending now " << std::flush; break;}
                    draw_array(vec.begin(), vec.end(), shader, window);
                    sleep(1);
                }

                std::cout << "\b\b\b\bnow " << std::endl;
                draw_array(vec.begin(), vec.end(), shader, window);
                break;

            case 1:    // bubble sort 
                std::cout << "\n\nperforming bubble sort on " << size << " elements..." << std::endl;
                draw_array(vec.begin(), vec.end(), shader, window);
                sleep(1);
                std::shuffle(vec.begin(), vec.end(), std::random_device{});
                sleep(1);
                seconds = static_cast<double>(benchmark([&](){bubble_sort(vec.begin(), vec.end(), shader, window);})) / (1e9);
                std::cout << "finished bubble sort in " << seconds << " seconds or " <<  seconds / 60.0 << " minutes" << std::endl;
                sleep(1);
                break;
            
            case 2:    // shaker sort 
                std::cout << "\n\nperforming shaker sort on " << size << " elements..." << std::endl;
                draw_array(vec.begin(), vec.end(), shader, window);
                sleep(1);
                std::shuffle(vec.begin(), vec.end(), std::random_device{});
                sleep(1);
                seconds = static_cast<double>(benchmark([&](){shaker_sort(vec.begin(), vec.end(), shader, window);})) / (1e9);
                std::cout << "finished shaker sort in " << seconds << " seconds or " <<  seconds / 60.0 << " minutes" << std::endl;
                sleep(1);
                break;

            case 3:  // selection sort
                std::cout << "\n\nperforming selection sort on " << size << " elements..." << std::endl;
                draw_array(vec.begin(), vec.end(), shader, window);
                sleep(1);
                std::shuffle(vec.begin(), vec.end(), std::random_device{});
                sleep(1);
                seconds = static_cast<double>(benchmark([&](){selection_sort(vec.begin(), vec.end(), shader, window);})) / (1e9);
                std::cout << "finished selection sort in " << seconds << " seconds or " <<  seconds / 60.0 << " minutes" << std::endl;
                sleep(1);
                break;
            
            case 4:  // insertion sort
                std::cout << "\n\nperforming insertion sort on " << size << " elements..." << std::endl;
                draw_array(vec.begin(), vec.end(), shader, window);
                sleep(1);
                std::shuffle(vec.begin(), vec.end(), std::random_device{});
                sleep(1);
                seconds = static_cast<double>(benchmark([&](){insertion_sort(vec.begin(), vec.end(), shader, window);})) / (1e9);
                std::cout << "finished insertion sort in " << seconds << " seconds or " <<  seconds / 60.0 << " minutes" << std::endl;
                sleep(1);
                break;
            
            case 5:  // quicksort
                std::cout << "\n\nperforming quicksort sort on " << size << " elements..." << std::endl;
                draw_array(vec.begin(), vec.end(), shader, window);
                sleep(1);
                std::shuffle(vec.begin(), vec.end(), std::random_device{});
                sleep(1);
                seconds = static_cast<double>(benchmark([&](){quicksort(vec.begin(), vec.end(), shader, window, vec.begin(), vec.end());})) / (1e9);
                std::cout << "finished insertion sort in " << seconds << " seconds or " <<  seconds / 60.0 << " minutes" << std::endl;
                sleep(1);
                break;


            default:
               std::cout << "something went wrong here i " << i << std::endl;
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
        float height = static_cast<float>(*i) / static_cast<float>(size);   // change to floats to avoid integer division
        glad_glUniform3f(glGetUniformLocation(shader->get_ID(), "color"), height, 0.0f, 1.0f - height);

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

std::vector<int> change_size(const int new_size, Shader* shader) {
    size = new_size;

    std::vector<int> vec(size);
    
    // populate array with every number in order
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = i + 1;
    }

    shader->setMat4("perspective", glm::ortho(-1.0f, static_cast<float>(size + 1), -1.0f, static_cast<float>(size + 1)));

    return vec;
}

/* SORTING ALGORITHMS */

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
                // draw after every write
                draw_array(first, last, shader, window);              
            }
        }
        
    }
}


template <class RandomIt>
void shaker_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window) {

    auto left_border = first;
    auto right_border = last;

    while (left_border < right_border) {
        // move left to right, pushing big items, biggest item is in place
        for (auto current = left_border; current < right_border - 1; ++current){
            if (*current > *(current + 1)){
                std::swap(*current, *(current + 1));
                draw_array(first, last, shader, window);
            }
        }
        --right_border;

        // move right to left, pushing small items, smallest item is in place
        for (auto current = right_border - 1; current > left_border; --current){
            if (*current < *(current - 1)){
                std::swap(*current, *(current - 1));
                draw_array(first, last, shader, window);
            }
        }
        ++left_border;
    }

}

template <class RandomIt>
void selection_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window)
{
    // first last represent bounds of unsorted array
    // first is the element being swapped with smallest value
    auto first_unsorted = first;
    while (first_unsorted < last)
    {
        // find the smallest item, by default is first element (as iterator)
        auto smallest = first_unsorted;

        for (auto current = first_unsorted + 1; current != last; ++current)
        {
            // if item is smaller, it is new smallest
            if (*current < *smallest)
            {
                smallest = current;
                draw_array(first, last, shader, window);
            }
        }

        // swap the values of first and smallest
        if (smallest != first_unsorted)
        {
            std::swap(*smallest, *first_unsorted);
            draw_array(first, last, shader, window);
        }

        ++first_unsorted;  // move to next unsorted element
    }
}

template <class RandomIt>
void insertion_sort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window)
{
    // iterate through unsorted array, starting from second element
    for (auto index = first + 1; index < last; ++index)
    {
        // store value of what is being inserted
        const auto current_val = *index;

        // store location of where to insert in sorted portion
        auto inserted_pos = index;

        // while not accessing first position and the next
        // value is lessthan current value
        while (inserted_pos > first && *(inserted_pos - 1) > current_val)
        {
            // shuffle larger value right one
            *inserted_pos = *(inserted_pos - 1);
            --inserted_pos;
            draw_array(first, last, shader, window);
        }

        // hole is made for value, now insert into place
        *inserted_pos = current_val;
        draw_array(first, last, shader, window);
    }
}


template <typename RandomIt>
void quicksort(RandomIt first, RandomIt last, const Shader* shader, GLFWwindow* window, RandomIt OG_first, RandomIt OG_last) {

    // base case. Return if only 1 element
    if (first >= last) {return;}

    // set pivot as last element
    // finding pivot in better way may improve performance
    const auto pivot_value = *(last - 1);

    // move all values larger than pivot to right of pivot
    // all values less than pivot to left of pivot

    // helps shuffle all values less than pivot to its left
    // also identifies final position of pivot
    RandomIt pivot_pos = first;

    for (RandomIt j = first; j != last - 1; ++j)
    {
        // if value in array is less than pivot value,
        // start stacking on left side of array
        if (*j < pivot_value)
        {
            std::swap(*pivot_pos, *j);
            ++pivot_pos;
            draw_array(OG_first, OG_last, shader, window);
        }
    }

    // swap first value larger than pivot with pivot
    std::swap(*pivot_pos, *(last - 1));
    draw_array(OG_first, OG_last, shader, window);

    // recursively do quicksort before and after pivot
    // do not include pivot itself, it is in final place
    quicksort(first, pivot_pos, shader, window, OG_first, OG_last);
    quicksort(pivot_pos + 1, last, shader, window, OG_first, OG_last);
}



/* EOF */
