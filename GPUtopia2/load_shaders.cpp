#include "load_shaders.h"


std::string loadShaderSourceFromFile(const std::string& filePath)
{
    std::string shaderSource;
    std::ifstream shaderFile;

    // Ensure ifstream objects can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderSource = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "ERROR: Failed to read shader file " << filePath << std::endl;
    }
    return shaderSource;
}

void deleteShaders(GLuint shaderProgram) {
    glDeleteProgram(shaderProgram);
}

GLuint compileAndLinkShaders(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
{
    // Compile and link the shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void loadShaders(GLuint& shaderProgram, const bool reload) {
    if (reload) deleteShaders(shaderProgram);
    std::string vertexShaderSourceStr = loadShaderSourceFromFile("vertex_shader.glsl");
    std::string fragmentShaderSourceStr = loadShaderSourceFromFile("fragment_shader.glsl");
    const GLchar* vertexShaderSource = vertexShaderSourceStr.c_str();
    const GLchar* fragmentShaderSource = fragmentShaderSourceStr.c_str();
    shaderProgram = compileAndLinkShaders(vertexShaderSource, fragmentShaderSource);
}

void prepBuffers(GLuint& VAO, GLuint& VBO, GLuint& EBO, GLuint& shaderProgram, const int Nx, const int Ny)
{
    loadShaders(shaderProgram);
    glUseProgram(shaderProgram);
    // Add this before the main loop
// Vertex data for a fullscreen quad
    float vertices[] = {
        -2.5f, -1.5f,
         1.5f, -1.5f,
         1.5f,  1.5f,
        -2.5f,  1.5f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLint resolutionLocation = glGetUniformLocation(shaderProgram, "resolution");
    glUniform2f(resolutionLocation, float(Nx), float(Ny));


    // Generate VAO and VBO
    //GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}