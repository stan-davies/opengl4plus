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

void update_fps_counter( GLFWwindow* window );

// updates window size data
void glfw_window_size_callback(GLFWwindow *window, int w, int h);

// sends glfw erros to gl.log
void glfw_error_callback(int error, const char* description);


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

void glfw_window_size_callback(GLFWwindow *window, int w, int h) {
        width = w;
        height = h;

        // update perspective matrices here
}

void glfw_error_callback(int error, const char *description) {
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
        GLuint program_id;
        load_program("shaders/default.vert", "shaders/default.frag", &program_id);

        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK);    // cull back fice
        glFrontFace(GL_CW);     // go clockwise

        // main running loop
        while(!glfwWindowShouldClose(window)) {
                update_fps_counter(window);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glViewport(0, 0, width, height);

                glUseProgram(program_id);

                glBindVertexArray(VAO);

                // draw points 0-4 from currently bound VAO with currently in-use shader
                glDrawArrays(GL_TRIANGLES, 0, vertices);

                glfwSwapBuffers(window);

                glfwPollEvents();

                if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                        glfwSetWindowShouldClose(window, 1);
                } else if (glfwGetKey(window, GLFW_KEY_R)) {
                        load_program("shaders/default.vert", "shaders/default.frag", &program_id);
                }
        }

        glfwTerminate();
        return 0;
}