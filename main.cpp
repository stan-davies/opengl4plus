#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

int main() {
    // start GL context using GLFW helper
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Traingle", NULL, NULL);

    if (NULL == window) {
        std::cerr << "ERROR: could not create window with GLFW" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // start GLEW, experimental encourages it to use newer (4.* +) versions of OpenGL
    glewExperimental = GL_TRUE;
    glewInit();

    // only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS);    // depth-testing interpretes a smaller value as "closer"

    float points[] = {
        -0.5f,  0.5f, 0.f,  // top left
         0.5f,  0.5f, 0.f,  // top right
         0.5f, -0.5f, 0.f   // bottom right
    };

    float other_points[] = {
         0.5f, -0.5f, 0.f,  // bottom right
        -0.5f, -0.5f, 0.f,  // bottom left
        -0.5f,  0.5f, 0.f   // top left
    };

    const int vertices = 3;
    const int dimension = 3;

    // create VBO and give it data
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices * dimension * sizeof(float), points, GL_STATIC_DRAW);

    // create VBO and give it data
    GLuint other_VBO = 0;
    glGenBuffers(1, &other_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, other_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices * dimension * sizeof(float), other_points, GL_STATIC_DRAW);

    // create VAO
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // enable first attribute, we only have one vertex buffer so we know that it is the start
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // defines layout of attribute 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // create VAO
    GLuint other_VAO = 0;
    glGenVertexArrays(1, &other_VAO);
    glBindVertexArray(other_VAO);
    // enable first attribute, we only have one vertex buffer so we know that it is the start
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, other_VBO);
    // defines layout of attribute 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // load and attach vertex shader
    std::string vertexString;
    std::ifstream vertexSource("shaders/vertex.glsl");
    if (!vertexSource) {
        std::cerr << "ERROR: file could not be loaded" << std::endl;
        return 1;
    }
    vertexString.assign((std::istreambuf_iterator<char>(vertexSource)), std::istreambuf_iterator<char>());
    const char* vertex_shader = vertexString.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    // load and attach fragment shader
    std::string fragment_string;
    std::ifstream fragment_source("shaders/fragment.glsl");
    if (!fragment_source) {
        std::cerr << "ERROR: file could not be loaded" << std::endl;
        return 1;
    }
    fragment_string.assign((std::istreambuf_iterator<char>(fragment_source)), std::istreambuf_iterator<char>());
    const char* fragment_shader = fragment_string.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    // load and attach other fragment shader
    std::string other_fragment_string;
    std::ifstream other_fragment_source("shaders/other_fragment.glsl");
    if (!other_fragment_source) {
        std::cerr << "ERROR: file could not be loaded" << std::endl;
        return 1;
    }
    other_fragment_string.assign((std::istreambuf_iterator<char>(other_fragment_source)), std::istreambuf_iterator<char>());
    const char* other_fragment_shader = other_fragment_string.c_str();

    GLuint ofs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(ofs, 1, &other_fragment_shader, NULL);
    glCompileShader(ofs);

    // create and link shader program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    GLuint other_shader_program = glCreateProgram();
    glAttachShader(other_shader_program, vs);
    glAttachShader(other_shader_program, ofs);
    glLinkProgram(other_shader_program);


    // main running loop
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glUseProgram(shader_program);

        glBindVertexArray(VAO);

        // draw points 0-4 from currently bound VAO with currently in-use shader
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);

        glUseProgram(other_shader_program);

        glBindVertexArray(other_VAO);

        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);

        glfwPollEvents();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}