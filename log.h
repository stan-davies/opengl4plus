#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <stdarg.h>

#define GL_LOG_FILE "gl.log"

bool restart_log() {
    FILE* file = fopen(GL_LOG_FILE, "w");
    if (!file) {
        fprintf(stderr, "ERROR: could not open log file for writing\n");
        return false;
    }

    time_t now = time(NULL);
    char* date = ctime(&now);
    // for some reason the date has a \n on the end of it
    fprintf(file, "GL_LOG_FILE log\nlocal time: %sversion: %s\n", date, VERSION);
    fclose(file);
    return true;
}

bool log(const char* message, ...) {
    va_list argptr;
    // append mode
    FILE* file = fopen(GL_LOG_FILE, "a");
    if (!file) {
        fprintf(stderr, "ERROR: could not open GL_LOG_FILE for appending\n");
        return false;
    }
    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);
    fclose(file);
    return true;
}

bool log_err(const char* message, ...) {
    va_list argptr;
    FILE* file = fopen(GL_LOG_FILE, "a");
    if (!file) {
        fprintf(stderr, "ERROR: could not open GL_LOG_FILE for appending\n");
        return false;
    }
    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);

    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);

    fclose(file);
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

    log("GL context parameters:\n");
    // integers - only works if the order is 0-10 integer return types
    for (int i = 0; i < 10; ++i) {
        int v = 0;
        glGetIntegerv(params[i], &v);
        log("%s %i\n", names[i], v);
    }

    // the others
    int v[2];
    v[0] = v[1] = 0;
    glGetIntegerv(params[10], v);
    log("%s %i %i\n", names[10], v[0], v[1]);

    unsigned char s = 0;
    glGetBooleanv(params[11], &s);
    log("%s %u\n", names[11], (unsigned int)s);
    log("---------------------------------------\n");
}

#endif