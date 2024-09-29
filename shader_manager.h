#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <string>
#include <GL/glew.h>
#include <fstream>
#include "log.h"

bool create_shader(GLenum type, std::string path, GLuint *shader_id) {
        log("shader");
        // open file and get ALL contents
        std::string shader_string;
        std::ifstream shader_source(path);
        if (!shader_source) {
                log_err("ERROR: shader at '", path, "' could not be loaded");
                return false;
        }

        shader_string.assign((std::istreambuf_iterator<char>(shader_source)), std::istreambuf_iterator<char>());
        const char *shader_c_str = shader_string.c_str();

        // create and compile shader
        GLuint id = glCreateShader(type);
        glShaderSource(id, 1, &shader_c_str, NULL);
        glCompileShader(id);

        delete[](shader_c_str);
        shader_c_str = nullptr;

        // check compile success
        int sh_cmp_stat = -1;
        glGetShaderiv(id, GL_COMPILE_STATUS, &sh_cmp_stat);
        if (GL_TRUE != sh_cmp_stat) {
                log_err("ERROR: fragment shader (id: ", id, ") did not compile");
                log_shader_logs(id);
                return false;
        }

        *shader_id = id;

        return true;
}

#endif