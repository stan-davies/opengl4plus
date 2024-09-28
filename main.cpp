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

// determines the current fps and inserts it into the window name
void update_fps_counter(GLFWwindow *window);

// sends shader logs to gl.log
void log_shader_logs(GLuint shader_id);

// sends program logs to gl.log
void log_program_logs(GLuint program_id);

// logs key data, logs all shader variables, calls log_program_logs
void program_log_all(GLuint program_id);

// converts Glenum shader dataypes to strings
const char* GLtype_to_string(GLenum type);

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
                // max size of the string for the name
                int max_length = 64;
                // actual size of the name
                int actual_length = 0;
                // size of the variable, in units of type, e.g. a vec2 would give a size of 2 (floats) whereas an int would give a size of 1 (int)
                int size = 0;
                // type of the variable
                GLenum type;

                glGetActiveAttrib(program_id, i, max_length, &actual_length, &size, &type, name);

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

        // load and attach vertex shader
        std::string vertexString;
        std::ifstream vertexSource("shaders/default.vert");
        if (!vertexSource) {
                log_err("ERROR: vertex shader could not be loaded");
                return 1;
        }
        vertexString.assign((std::istreambuf_iterator<char>(vertexSource)), std::istreambuf_iterator<char>());
        const char *vertex_shader = vertexString.c_str();

        GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs_id, 1, &vertex_shader, NULL);
        glCompileShader(vs_id);
        // check if compilation worked
        int vs_comp_status = -1;
        glGetShaderiv(vs_id, GL_COMPILE_STATUS, &vs_comp_status);
        if (GL_TRUE != vs_comp_status) {
                log_err("ERROR: vertex shader (id: ", vs_id, ") did not compile");
                log_shader_logs(vs_id);
                return false;
        }

        // load and attach fragment shader
        std::string fragment_string;
        std::ifstream fragment_source("shaders/default.frag");
        if (!fragment_source) {
                log_err("ERROR: fragment shader could not be loaded");
                return 1;
        }
        fragment_string.assign((std::istreambuf_iterator<char>(fragment_source)), std::istreambuf_iterator<char>());
        const char *fragment_shader = fragment_string.c_str();

        GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs_id, 1, &fragment_shader, NULL);
        glCompileShader(fs_id);
        // check if compilation worked
        int fs_comp_status = -1;
        glGetShaderiv(fs_id, GL_COMPILE_STATUS, &fs_comp_status);
        if (GL_TRUE != fs_comp_status) {
                log_err("ERROR: fragment shader (id: ", fs_id, ") did not compile");
                log_shader_logs(fs_id);
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