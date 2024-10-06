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

bool create_program(int shader_count, GLuint shaders[], GLuint *_program_id) {
        GLuint program_id = glCreateProgram();
        for (int i = 0; i < shader_count; ++i) {
                glAttachShader(program_id, shaders[i]);
        }
        glLinkProgram(program_id);

        // check if link worked
        int link_status = -1;
        glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
        log("program ", program_id, " GL_LINK_STATUS = ", link_status);
        if (GL_TRUE != link_status) {
                log_err("ERROR: could not link shader program (id: ", program_id, ")");
                log_program_logs(program_id);
                return false;
        }

        // check if program is valid
        glValidateProgram(program_id);
        int valid = -1;
        glGetProgramiv(program_id, GL_VALIDATE_STATUS, &valid);
        log("program ", program_id, " GL_VALIDATE_STATUS = ", valid);
        if (GL_TRUE != valid) {
                log_err("ERROR: program ", program_id, " is not valid");
                log_program_logs(program_id);
                return false;
        }

        *_program_id = program_id;
        return true;
} 

bool load_program(const char *vertex_path, const char *fragment_path, GLuint *_program_id) {
        GLuint v_id;
        if (!create_shader(GL_VERTEX_SHADER, vertex_path, &v_id)) {
                log_err("ERROR: vertex shader creation failed");
                return false;
        }

        GLuint f_id;
        if (!create_shader(GL_FRAGMENT_SHADER, fragment_path, &f_id)) {
                log_err("ERROR: fragment shader creation failed");
                return false;
        }

        GLuint shaders[2];
        shaders[0] = v_id;
        shaders[1] = f_id;

        GLuint program_id;
        if (!create_program(2, shaders, &program_id)) {
                log_err("ERROR: shader program creation failed");
                return false;
        }

        *_program_id = program_id;
        return true;
}

#endif