#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "log.h"

#include "shader_manager.h"

int width = 640;
int height = 480;

static double previous_seconds;

// determines the current fps and inserts it into the window name
void update_fps_counter(GLFWwindow *window);

// updates window size data
void glfw_window_size_callback(GLFWwindow *window, int w, int h);

// sends glfw erros to gl.log
void glfw_error_callback(int error, const char* description);

// checks if a program is valid
bool is_valid(GLuint program_id);


void update_fps_counter(GLFWwindow *window) {
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

void glfw_window_size_callback(GLFWwindow *window, int w, int h) {
        width = w;
        height = h;

        // update perspective matrices here
}

void glfw_error_callback(int error, const char *description) {
        log_err("GLFW ERROR: code ", error, " msg: ", description);
}

bool is_valid(GLuint program_id) {
        glValidateProgram(program_id);
        int valid = -1;
        glGetProgramiv(program_id, GL_VALIDATE_STATUS, &valid);
        log("program ", program_id, " GL_VALIDATE_STATUS = ", valid);
        if (GL_TRUE != valid) {
                log_program_logs(program_id);
                return false;
        }

        return true;
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
        GLFWwindow *window = glfwCreateWindow(width, height, "Extended Init", NULL, NULL);

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
        const GLubyte *renderer = glGetString(GL_RENDERER);
        const GLubyte *version = glGetString(GL_VERSION);
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


        GLuint vs_id = 0;
        if (!create_shader(GL_VERTEX_SHADER, "shaders/default.vert", &vs_id)) {
                return false;
        }

        GLuint fs_id = 0;
        if (!create_shader(GL_FRAGMENT_SHADER, "shaders/default.frag", &fs_id)) {
                return false;
        }

        // create and link shader program
        GLuint shader_program_id = glCreateProgram();
        glAttachShader(shader_program_id, fs_id);
        glAttachShader(shader_program_id, vs_id);
        glLinkProgram(shader_program_id);
        // check if link worked
        int link_status = -1;
        glGetProgramiv(shader_program_id, GL_LINK_STATUS, &link_status);
        if (GL_TRUE != link_status) {
                log_err("ERROR: could not link shader program (id: ", shader_program_id, ")");
                log_program_logs(shader_program_id);
                return false;
        }

        if (!is_valid(shader_program_id)) {
                log_err("ERROR: program ", shader_program_id, " is not valid");
        }

        // gets location of a uniform
        // all uniforms are initialised to 0
        // sometimes, this is okay, for other, maybe not
        // also this is an expensive operation
        GLint input_colour_loc = glGetUniformLocation(shader_program_id, "input_colour");
        if (-1 == input_colour_loc) {
                log_err("ERROR: could not find location of uniform 'input_colour'");
                program_log_all(shader_program_id);
        }

        // avoid altering attributes during run-time, because it is super expensive apparently =O

        program_log_all(shader_program_id);

        // main running loop
        while(!glfwWindowShouldClose(window)) {
                update_fps_counter(window);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, width, height);
        
                glUseProgram(shader_program_id);

                // expensive operation!!
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