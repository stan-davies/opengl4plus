#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "log.h"

int width = 640;
int height = 480;

static double previous_seconds;

void _update_fps_counter(GLFWwindow* window) {
    static int frame_count;
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25) {
        previous_seconds = current_seconds;
        double fps = (double)frame_count / elapsed_seconds;
        char tmp[128];
        sprintf(tmp, "opengl @ fps: %.2f", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }
    frame_count++;
}

void glfw_window_size_callback(GLFWwindow* window, int w, int h) {
    width = w;
    height = h;

    // update perspective matrices here
}

void glfw_error_callback(int error, const char* description) {
    log_err("GLFW ERROR: code ", error, " msg: ", description);
}

int main() {
    // get that long file a'rollin
    restart_log();

    log("starting GLFW with version: ", glfwGetVersionString());

    // tell GLFW how to log errors
    glfwSetErrorCallback(glfw_error_callback);

    // start GL context using GLFW helper
    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        return 1;
    }

    // put any window hints here
    // this one sets anti-aliasing passes to 4
    // 16 is a good level for screenshots
    glfwWindowHint(GLFW_SAMPLES, 4);

    // # set the window to fill the main monitor
    // GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    // const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
    // GLFWwindow* window = glfwCreateWindow(video_mode->width, video_mode->height, "Extended Init", monitor, NULL);
    // # set the window to be a little window
    GLFWwindow* window = glfwCreateWindow(width, height, "Extended Init", NULL, NULL);

    if (NULL == window) {
        std::cerr << "ERROR: could not create window with GLFW" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    
    // tell GLFW what to do when the window is resized
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    // start GLEW, experimental encourages it to use newer (4.* +) versions of OpenGL
    glewExperimental = GL_TRUE;
    glewInit();

    // some key data for the log
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    log("renderer: ", renderer, "\nOpenGL version supported: ", version);
    log_gl_params();
    delete renderer;
    delete version;
    renderer = nullptr;
    version = nullptr;

    // only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);    // depth-testing interpretes a smaller value as "closer"

    float points[] = {
        -0.5f,  0.5f, 0.f,  // top left
         0.5f,  0.5f, 0.f,  // top right
         0.5f, -0.5f, 0.f   // bottom right
    };

    const int vertices = 3;
    const int dimension = 3;

    // create VBO and give it data
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices * dimension * sizeof(float), points, GL_STATIC_DRAW);

    // create VAO
    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // enable first attribute, we only have one vertex buffer so we know that it is the start
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // defines layout of attribute 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // load and attach vertex shader
    std::string vertexString;
    std::ifstream vertexSource("shaders/default.vert");
    if (!vertexSource) {
        log_err("ERROR: vertex shader 1 could not be loaded");
        return 1;
    }
    vertexString.assign((std::istreambuf_iterator<char>(vertexSource)), std::istreambuf_iterator<char>());
    const char* vertex_shader = vertexString.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    // load and attach fragment shader
    std::string fragment_string;
    std::ifstream fragment_source("shaders/default.frag");
    if (!fragment_source) {
        log_err("ERROR: vertex shader 2 could not be loaded");
        return 1;
    }
    fragment_string.assign((std::istreambuf_iterator<char>(fragment_source)), std::istreambuf_iterator<char>());
    const char* fragment_shader = fragment_string.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    // create and link shader program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    GLint input_colour_loc = glGetUniformLocation(shader_program, "input_colour");

    // main running loop
    while(!glfwWindowShouldClose(window)) {
        _update_fps_counter(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width, height);
    
        glUseProgram(shader_program);

        glUniform4f(input_colour_loc, 1.f, 0.f, 0.f, 1.f);

        glBindVertexArray(VAO);

        // draw points 0-4 from currently bound VAO with currently in-use shader
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    glfwTerminate();
    return 0;
}