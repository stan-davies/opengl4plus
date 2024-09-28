#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <stdarg.h>
#include <fstream>
#include "version.h"

#define GL_LOG_FILE "gl.log"

bool restart_log() {
        // open file for writing
        std::fstream file;
        file.open(GL_LOG_FILE, std::fstream::out);

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
        std::fstream file;
        file.open(GL_LOG_FILE, std::fstream::app);

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
        std::fstream file;
        file.open(GL_LOG_FILE, std::fstream::app);

        if (!file) {
                std::cerr << "ERROR: could not open GL_LOG_FILE for appending" << std::endl;
                return false;
        }

        // log variable amount of stuff
        (file << ... << args);
        file << std::endl;

        // directly output it too because it's an error and we want to know about it!
        std::cerr << "ERROR: see logs for more information" << std::endl;

        file.close();
        return true;
}

void log_gl_params() {
        GLenum params[] = {
                GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
                GL_MAX_CUBE_MAP_TEXTURE_SIZE,
                GL_MAX_DRAW_BUFFERS,
                GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
                GL_MAX_TEXTURE_IMAGE_UNITS,
                GL_MAX_TEXTURE_SIZE,
                GL_MAX_VARYING_FLOATS,
                GL_MAX_VERTEX_ATTRIBS,
                GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
                GL_MAX_VERTEX_UNIFORM_COMPONENTS,
                GL_MAX_VIEWPORT_DIMS,
                GL_STEREO
        };

        const char* names[] = {
                "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
                "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
                "GL_MAX_DRAW_BUFFERS",
                "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
                "GL_MAX_TEXTURE_IMAGE_UNITS",
                "GL_MAX_TEXTURE_SIZE",
                "GL_MAX_VARYING_FLOATS",
                "GL_MAX_VERTEX_ATTRIBS",
                "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
                "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
                "GL_MAX_VIEWPORT_DIMS",
                "GL_STEREO"
        };

        log("GL context parameters:");
        // integers - only works if the order is 0-10 integer return types
        for (int i = 0; i < 10; ++i) {
                int v = 0;
                glGetIntegerv(params[i], &v);
                log(names[i], " ", v);
        }

        // the others
        int v[2];
        v[0] = v[1] = 0;
        glGetIntegerv(params[10], v);
        log(names[10], " ", v[0], " ", v[1]);

        unsigned char s = 0;
        glGetBooleanv(params[11], &s);
        log(names[11], " ", (unsigned int)s);
        log("---------------------------------------");
}

#endif