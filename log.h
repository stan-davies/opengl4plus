#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <stdarg.h>
#include <fstream>
#include "version.h"

#define GL_LOG_FILE "gl.log"

// sends shader logs to gl.log
void log_shader_logs(GLuint shader_id);

// sends program logs to gl.log
void log_program_logs(GLuint program_id);

// logs key data, logs all shader variables, calls log_program_logs
void program_log_all(GLuint program_id);

// converts Glenum shader dataypes to strings
const char* GLtype_to_string(GLenum type);

bool restart_log() {
        // open file for writing
        std::fstream file(GL_LOG_FILE, std::fstream::out);

        if (!file) {
                std::cerr << "ERROR: could not open log file for writing" << std::endl;
                return false;
        }

        time_t now = time(NULL);
        char* date = ctime(&now);
        // for some reason the date has a \n on the end of it so don't worry about that
        file << "GL_LOG_FILE log\nlocal time: " << date << "version: " << VERSION << std::endl;
        file.close();
        return true;
}

// ummmm find out how this works lol
template<class... Args>
bool log(Args... args) {
        // append mode
        std::fstream file(GL_LOG_FILE, std::fstream::app);

        if (!file) {
                std::cerr << "ERROR: could not open GL_LOG_FILE for appending" << std::endl;
                return false;
        }

        // log variable amount of stuff
        (file << ... << args);
        file << std::endl;

        file.close();

        return true;
}

template<class... Args>
bool log_err(Args... args) {
        // open file for appending
        std::fstream file(GL_LOG_FILE, std::fstream::app);

        if (!file) {
                std::cerr << "ERROR: could not open GL_LOG_FILE for appending" << std::endl;
                return false;
        }

        // log variable amount of stuff
        (file << ... << args);
        file << std::endl;

        // directly output it too because it's an error and we want to know about it!
        (std::cerr << ... << args);
        std::cerr << std::endl;

        file.close();
        return true;
}

void log_shader_logs(GLuint shader_id) {
        // we need all the these variables just because the function takes them
        // max_length and shader_log are actually useful, they tell the function how many characters to give us
        // actual length has a value put into it, so if our logging used only C strings, we would be pretty chuffed with that
        // but i'm not using C strings so it is a little pointless
        int max_length = 2048;
        int actual_length = 0;
        char shader_log[2048];
        glGetShaderInfoLog(shader_id, max_length, &actual_length, shader_log);
        log("shader info log for ", shader_id, ": ", shader_log);
}

void log_program_logs(GLuint program_id) {
        // see comments in _log_shader_logs
        int max_length = 2048;
        int actual_length = 0;
        char program_log[2048];
        glGetProgramInfoLog(program_id, max_length, &actual_length, program_log);
        log("program info log for ", program_id, ": ", program_log);
}

void program_log_all(GLuint program_id) {
        log("--------------------\nshader program ", program_id, " info:");

        int data = -1;

        // log some key data
        glGetProgramiv(program_id, GL_LINK_STATUS, &data);
        log("GL_LINK_STATUS = ", data);

        glGetProgramiv(program_id, GL_ATTACHED_SHADERS, &data);
        log("GL_ATTACHED_SHADERS = ", data);

        glGetProgramiv(program_id, GL_ACTIVE_ATTRIBUTES, &data);
        log("GL_ACTIVE_ATTRIBUTES = ", data);

        // log all variables that have been included in the compile
        // this is signficant as not all variables will get compiled
        // for example, unused ones may be discarded for efficiency
        // however, you may forget to remove C++ code that asks for their locations
        // and you will still see that you wrote them in the GLSL script
        // so if we log what the computer believes there is, then all should be well!
        for (int i = 0; i < data; ++i) {
                // string for the name
                char name[64];
                // actual size of the name
                int actual_length = 0;
                // size of the variable, in units of type, e.g. a vec2 would give a size of 2 (floats) whereas an int would give a size of 1 (int)
                int size = 0;
                // type of the variable
                GLenum type;

                glGetActiveAttrib(program_id, i, 64, &actual_length, &size, &type, name);

                if (size > 1) {
                for (int j = 0; j < size; ++j) {
                        char indexed_name[64];
                        // String PRINT Formatted
                        // gives long_name the value as specified in the latter arguments
                        // for example it might create "output_colour[1]"
                        sprintf(indexed_name, "%s[%i]", name, j);
                        int location = glGetAttribLocation(program_id, indexed_name);
                        log("  ", i, ") type:", GLtype_to_string(type), " name:", indexed_name, " location:", location);
                }
                } else {
                int location = glGetAttribLocation(program_id, name);
                log("  ", i, ") type:", GLtype_to_string(type), " name:", name, " location:", location);
                }
        }

        glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &data);
        log("GL_ACTIVE_UNIFORMS = ", data);

        for (int i = 0; i < data; ++i) {
                // string for the name
                char name[64];
                // max size of the string for the name
                int max_length = 64;
                // actual size of the name
                int actual_length = 0;
                // size of the variable, in units of type, e.g. a vec2 would give a size of 2 (floats) whereas an int would give a size of 1 (int)
                int size = 0;
                // type of the variable
                GLenum type;

                glGetActiveUniform(program_id, i, max_length, &actual_length, &size, &type, name);

                if (size > 1) {
                for (int j = 0; j < size; ++j) {
                        char indexed_name[64];
                        // String PRINT Formatted
                        // gives long_name the value as specified in the latter arguments
                        // for example it might create "output_colour[1]"
                        sprintf(indexed_name, "%s[%i]", name, j);
                        int location = glGetUniformLocation(program_id, indexed_name);
                        log("  ", i, ") type:", GLtype_to_string(type), " name:", indexed_name, " location:", location);
                }
                } else {
                int location = glGetUniformLocation(program_id, name);
                log("  ", i, ") type:", GLtype_to_string(type), " name:", name, " location:", location);
                }
        }

        log_program_logs(program_id);
}

const char *GLtype_to_string(GLenum type) {
        switch (type) {
        case GL_BOOL : return "bool";
        case GL_INT : return "int";
        case GL_FLOAT : return "float";
        case GL_FLOAT_VEC2   : return "vec2";
        case GL_FLOAT_VEC3   : return "vec3";
        case GL_FLOAT_VEC4   : return "vec4";
        case GL_FLOAT_MAT2   : return "mat2";
        case GL_FLOAT_MAT3   : return "mat3";
        case GL_FLOAT_MAT4   : return "mat4";
        case GL_SAMPLER_2D   : return "sampler2D";
        case GL_SAMPLER_3D   : return "sampler3D";
        case GL_SAMPLER_CUBE : return "samplerCube";
        case GL_SAMPLER_2D_SHADOW : return "sampler2DShadow";
        default: break;
        }

        return "uncommon-data-type";
}

#endif