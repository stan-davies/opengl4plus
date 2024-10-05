#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include "log.h"

#include "util.h"

#include "shader_manager.h"
#include "texture_manager.h"

int main() {
        if (!init()) {
                log_err("ERROR: failed to initialise");
                return 0;
        }

        if (nullptr == window) {
                log_err("ERROR: window failed =/");
                return 0;
        }

        
        // DATA TIME!!!
        float vertices[] = {
        //       x      y     z          s    t
                 1.0f,  1.0f, 0.0f,      1.0f, 1.0f, // top right
                 1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // bottom right
                -1.0f, -1.0f, 0.0f,      0.0f, 0.0f, // bottom left
                -1.0f,  1.0f, 0.0f,      0.0f, 1.0f  // top left
        };

        const int vertices_size = sizeof(vertices);

        unsigned int indices[] = {
                0, 1, 3,  // first triangle
                1, 2, 3   // second triangle
        };

        const int indices_size = sizeof(indices);

        // create VAO
        GLuint VAO = 0;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // create VBO and give it data
        GLuint VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

        GLuint IBO = 0;
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

        // positions
        // 8 * sizeof(float) is the stride
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // texture coords
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // for some reason, buffers are disabled by default, so enable them!
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        

        // texture time!!!
        GLuint tex_1_id = 0;
        if (!create_texture("container.jpg", &tex_1_id, GL_RGB)) {
                log_err("ERROR: texture could not be loaded");
                glfwTerminate();
                return false;
        }


        



        




        // PROGRAM TIME!!
        GLuint program_id;
        if (!load_program("shaders/default.vert", "shaders/default.frag", &program_id)) {
                log_err("ERROR: could not load program");
                glfwTerminate();
                return false;
        }


        glEnable(GL_CULL_FACE); // cull face
        glCullFace(GL_BACK);    // cull back face
        glFrontFace(GL_CW);     // go clockwise


        // set which texture unit each sampler needs to use
        glUseProgram(program_id);
        glUniform1i(glGetUniformLocation(program_id, "tex1"), 0);


        // main running loop
        while(!glfwWindowShouldClose(window)) {
                update_fps_counter(window);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glViewport(0, 0, width, height);

                // activate texture units and bind them
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex_1_id);

                glUseProgram(program_id);
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                glfwSwapBuffers(window);

                glfwPollEvents();

                if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                        glfwSetWindowShouldClose(window, 1);
                } else if (glfwGetKey(window, GLFW_KEY_R)) {
                        if (!load_program("shaders/default.vert", "shaders/default.frag", &program_id)) {
                                log_err("ERROR: could not load program");
                        }
                }
        }

        glfwTerminate();
        return 0;
}