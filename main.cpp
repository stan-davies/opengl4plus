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

void update_fps_counter( GLFWwindow* window ) {
        static double previous_seconds = glfwGetTime();
        static int frame_count;
        double current_seconds = glfwGetTime();
        double elapsed_seconds = current_seconds - previous_seconds;
        if ( elapsed_seconds > 0.25 ) {
                previous_seconds = current_seconds;
                double fps = (double)frame_count / elapsed_seconds;
                char tmp[128];
                sprintf( tmp, "opengl @ fps: %.2f", fps );
                glfwSetWindowTitle( window, tmp );
                frame_count = 0;
        }
        frame_count++;
}

// updates window size data
void glfw_window_size_callback(GLFWwindow *window, int w, int h);

// sends glfw erros to gl.log
void glfw_error_callback(int error, const char* description);

// checks if a program is valid
bool is_valid(GLuint program_id);

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
                log_err("ERROR: could not start GLFW");
                return 1;
        }

        // put any window hints here
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        // this one sets anti-aliasing passes to 4
        // 16 is a good level for screenshots
        glfwWindowHint( GLFW_SAMPLES, 4 );

        GLFWwindow *window = glfwCreateWindow(width, height, "VAO VBO", NULL, NULL);

        if (NULL == window) {
                log_err("ERROR: could not create window with GLFW");
                glfwTerminate();
                return 1;
        }

        glfwMakeContextCurrent(window);
        
        // tell GLFW what to do when the window is resized
        glfwSetFramebufferSizeCallback(window, glfw_window_size_callback);

        // start GLEW, experimental encourages it to use newer (4.* +) versions of OpenGL
        glewExperimental = GL_TRUE;
        glewInit();

        // some key data for the log
        const GLubyte *renderer = glGetString(GL_RENDERER);
        const GLubyte *version = glGetString(GL_VERSION);
        log("renderer: ", renderer, "\nOpenGL version supported: ", version);
        delete renderer;
        delete version;
        renderer = nullptr;
        version = nullptr;

        log("-------------");

        // only draw onto a pixel if the shape is closer to the viewer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);    // depth-testing interpretes a smaller value as "closer"


        // DATA TIME!!!
        float points[] = {
                 0.f,   0.5f, 0.f,  // top middle
                 0.5f, -0.5f, 0.f,  // bottom right
                -0.5f, -0.5f, 0.f   // bottom left
        };

        float colours[] = {
                1.f, 0.f, 0.f,
                0.f, 1.f, 0.f,
                0.f, 0.f, 1.f
        };

        const int vertices = 3;
        const int dimension = 3;

        // create VBO and give it data
        GLuint VBO_p = 0;
        glGenBuffers(1, &VBO_p);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
        glBufferData(GL_ARRAY_BUFFER, vertices * dimension * sizeof(float), points, GL_STATIC_DRAW);

        GLuint VBO_c = 0;
        glGenBuffers(1, &VBO_c);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
        glBufferData(GL_ARRAY_BUFFER, vertices * dimension * sizeof(float), colours, GL_STATIC_DRAW);

        // create VAO
        GLuint VAO = 0;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_p);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_c);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // for some reason, buffers are disabled by default, so enable them!
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);


        // PROGRAM TIME!!
        bool success;

        GLuint vs_id = 0;
        success = create_shader(GL_VERTEX_SHADER, "shaders/default.vert", &vs_id);
        if (!success || 0 == vs_id) {
                log_err("ERROR: vertex shader creation failed");
                return false;
        }

        GLuint fs_id = 0;
        success = create_shader(GL_FRAGMENT_SHADER, "shaders/default.frag", &fs_id);
        if (!success || 0 == fs_id) {
                log_err("ERROR: fragment shader creation failed");
                return false;
        }

        log("vs: ", vs_id, " fs: ", fs_id);

        // create and link shader program
        GLuint shader_program_id = glCreateProgram();
        glAttachShader(shader_program_id, fs_id);
        glAttachShader(shader_program_id, vs_id);
        glLinkProgram(shader_program_id);
        // check if link worked
        int link_status = -1;
        glGetProgramiv(shader_program_id, GL_LINK_STATUS, &link_status);
        log("program ", shader_program_id, " GL_LINK_STATUS = ", link_status);
        if (GL_TRUE != link_status) {
                log_err("ERROR: could not link shader program (id: ", shader_program_id, ")");
                log_program_logs(shader_program_id);
                return false;
        }

        if (!is_valid(shader_program_id)) {
                log_err("ERROR: program ", shader_program_id, " is not valid");
                return false;
        }

        // main running loop
        while(!glfwWindowShouldClose(window)) {
                update_fps_counter(window);
                log("1");
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, width, height);
                log("2");
                glUseProgram(shader_program_id);
                log("3");
                glBindVertexArray(VAO);
                log("4");
                // draw points 0-4 from currently bound VAO with currently in-use shader
                glDrawArrays(GL_TRIANGLES, 0, vertices);
                log("5");
                glfwSwapBuffers(window);
                log("6");

                glfwPollEvents();
                log("7");
                if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                        glfwSetWindowShouldClose(window, 1);
                }
        }

        glfwTerminate();
        return 0;
}