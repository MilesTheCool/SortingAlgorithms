/// @author Miles Rasmussen
/// @brief A class to handle OpenGL shaders. Given the file paths, 
/// it will read in the vertex and fragment shaders, compile them, 
/// and link them. If there are any errors, they will be displayed to 
/// the terminal (such as compiler or linking errors)
/// @date 5/31/24 - First version created
/// @date 6/4/24 - added StatusEnum to track if an invalid shader is made
/// also added a default constructor to better work with object classes

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>   // get access to OpenGL headers

#include <string>
#include <fstream>
#include <sstream> 
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader
{
private:

    // track if there is an invalid shader object, so it does not attempt to access shader functions
    // allows the creation of a default constructor
    enum StatusEnum {VALID_SHADERS, INVALID_SHADERS};
    StatusEnum status;

    unsigned int ID;   // make private to avoid accidental changes from client

    /// @brief helper-function to aid the construtor. Reads in the 
    /// shader files and returns them as strings for the constructor to compile
    /// @param vertexPath - the file path to the vertex shader
    /// @param fragmentPath - the file path to the fragment shader
    /// @param vertexCode [out] - the string code of the vertex shader
    /// @param fragmentCode [out] - the string code of the fragment shader
    void read_files(const char* vertexPath, const char* fragmentPath,
    std::string &vertexCode, std::string &fragmentCode);

    /// @brief  compile and link the shaders, and create the OpenGL Program
    /// @param vertexCode - the c-string containing the entire vertex shader code
    /// @param fragmentCode  - the c-string containing the entire fragment shader code
    void compile_shaders(const char* vertexCode, const char* fragmentCode);

public:
    enum ConstructorType {USING_FILE_PATHS, USING_SHADER_STRING};
    
    // constructor to read and compile the shader
    Shader() : status(INVALID_SHADERS) {}
    Shader(const char* vertexPath, const char* fragmentPath, ConstructorType consType=USING_FILE_PATHS);

    ~Shader();

    void deleteResources();

    // set the shader as active
    void use() const {glUseProgram(ID);}

    // give read-only ID
    unsigned int get_ID() const {return ID;}

    // utility uniform functions
    // same as calling FUNCTION(glGetUniformLocation(shader, "uniform_name"),...)
    // for all glUniform functions
    void setBool(const std::string &name, bool value) const
    {
        if (status == INVALID_SHADERS) {return;}  // return if in invalid state
        glUniform1i(glGetUniformLocation(ID, name.c_str()), 
                                  static_cast<int>(value));
    }
    void setInt(const std::string &name, int value) const
    {
        if (status == INVALID_SHADERS) {return;}  // return if in invalid state
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        if (status == INVALID_SHADERS) {return;}  // return if in invalid state
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setMat4(const std::string &name, glm::mat4 value) const
    {
        if (status == INVALID_SHADERS) {return;}  // return if in invalid state
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setVec3(const std::string &name, glm::vec3 &value){
        if (status == INVALID_SHADERS) {return;}  // return if in invalid state
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
    }
};


Shader::Shader(const char* vertexPath, const char* fragmentPath, ConstructorType consType)
{
    status = VALID_SHADERS;

    if (consType == USING_FILE_PATHS) {
        /* 1. retrieve the vertex/fragment source code from filePath */
        std::string vertexCode, fragmentCode;       // string holding entire code

        read_files(vertexPath, fragmentPath, vertexCode, fragmentCode);
        
        // convert from std::string to c-string
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        /* 2. Compile Shaders */
        compile_shaders(vShaderCode, fShaderCode);
    } else {
        /* Compile Shader Directly*/
        compile_shaders(vertexPath, fragmentPath);
    }
    
    
}

Shader::~Shader() {
    deleteResources();
}

void Shader::deleteResources() {
    if (status == VALID_SHADERS) {
        status = INVALID_SHADERS;
        glDeleteProgram(ID);
    }
}

void Shader::read_files(const char* vertexPath, const char* fragmentPath,
    std::string &vertexCode, std::string &fragmentCode)
{
    std::ifstream vShaderFile, fShaderFile;     // infile streams

    // ensure ifstreams can throw exceptions
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close files
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // set status to being valid
        status = VALID_SHADERS;
    }
    catch(std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n\t"
                  << e.what() << std::endl;
        status = INVALID_SHADERS;
    }
}

void Shader::compile_shaders(const char* vertexCode, const char* fragmentCode)
{
    if (status == INVALID_SHADERS) {return;}  // return if in invalid state

    unsigned int vertex, fragment;   // opengl ID for shaders
    int success;                     // flag compile error
    char infoLog[512];               // log to display compiler messages

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    //print any compiler errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        status = INVALID_SHADERS;
    }

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    //print compiler errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        status = INVALID_SHADERS;
    }

    // shader program
    ID = glCreateProgram();  // sets the private-member variable ID
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print any linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        status = INVALID_SHADERS;
    }

    // delete the shaders cuz their linked, so we can free the memory
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

#endif  // closing include guard
/* EOF */
