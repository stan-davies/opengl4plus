#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "log.h"

#include "util.h"

#include "shader_manager.h"
#include "texture_manager.h"

int main() {
        // lets get going
        if (!init()) {
                log_err("ERROR: failed to initialise");
                return 0;
        }

        
        // DATA TIME!!!
        float vertices[] = {
        //       x      y          s    t
                -1.0f, -1.0f,      0.0f, 0.0f, // bottom left
                -1.0f,  1.0f,      0.0f, 1.0f, // top left
                 1.0f, -1.0f,      1.0f, 0.0f, // bottom right
                 1.0f,  1.0f,      1.0f, 1.0f, // top right
        };

        const int vertices_size = sizeof(vertices);

        // create VAO
        GLuint VAO = 0;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // create VBO and give it data
        GLuint VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

        // positions
        // 5 * sizeof(float) is the stride
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // texture coords
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // for some reason, buffers are disabled by default, so enable them!
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        


        


        

        // compute time

        // get that fella alive
        GLuint ray_shader_id;
        create_shader(GL_COMPUTE_SHADER, "shaders/tracer.comp", &ray_shader_id);

        GLuint ray_program_id;
        GLuint comp_sh[1];
        comp_sh[0] = ray_shader_id;
        create_program(1, comp_sh, &ray_program_id);
        delete[](comp_sh);



        // texture time!!!
        GLuint tex_id = 0;
        int tex_w = 512;
        int tex_h = 512;
        if (!create_texture(&tex_id, tex_w, tex_h)) {
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


        // set which texture unit each sampler needs to use
        // glUseProgram(program_id);
        // glUniform1i(glGetUniformLocation(program_id, "img"), 0);

        glClearColor(1.0, 0.0, 0.0, 1.0);


        // main running loop
        while(!glfwWindowShouldClose(window)) {
                { // launch compute shaders
                        glUseProgram(ray_program_id);
                        // dispatch in (image width, image height, 1) so that each pixel has a shader
                        glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
                }

                // make sure writing to image has finished before it is read from 
                glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

                { // regular drawing pass
                        update_fps_counter(window);

                        glClear(GL_COLOR_BUFFER_BIT);

                        glUseProgram(program_id);

                        glViewport(0, 0, width, height);
                        
                        glBindVertexArray(VAO);

                        // activate texture units and bind them
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, tex_id);

                        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                }

                glfwPollEvents();

                if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                        glfwSetWindowShouldClose(window, 1);
                } else if (glfwGetKey(window, GLFW_KEY_R)) {
                        if (!load_program("shaders/default.vert", "shaders/default.frag", &program_id)) {
                                log_err("ERROR: could not load program");
                        }
                }

                glfwSwapBuffers(window);
        }

        glfwTerminate();
        return 0;
}