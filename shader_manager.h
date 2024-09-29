#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

// 1028 * 256 = 262144
#define MAX_SHADER_LENGTH 262144

bool parse_file(const char *path, char *string, int max_len) {
        FILE *file = fopen(path, "r");
        if (!file) {
                return false;
        }

        size_t cnt_len = fread(string, 1, max_len - 1, file);
        if ((int)cnt_len >= max_len - 1) {
                log_err("WARNING: file at '", path, "' is too long, will be truncated");
        }

        if (ferror(file)) {
                log_err("ERROR: reading file at '", path, "'");
                return false;
        }

        // append \0 to end of file string
        string[cnt_len] = 0;
        fclose(file);
        return true;
}

bool create_shader(GLenum type, const char *path, GLuint *shader_id) {
        // open file and get ALL contents
        char shader_string[MAX_SHADER_LENGTH];
        bool success = parse_file(path, shader_string, MAX_SHADER_LENGTH);
        if (!success) {
                log_err("ERROR: could not open file at '", path, "'");
                return false;
        }

        const GLchar *gl_string = (const GLchar *)shader_string;

        // create and compile shader
        GLuint id = glCreateShader(type);
        glShaderSource(id, 1, &gl_string, NULL);
        glCompileShader(id);

        delete[](gl_string);
        gl_string = nullptr;

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