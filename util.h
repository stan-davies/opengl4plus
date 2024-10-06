#ifndef UTIL_H
#define UTIL_H

int width = 512;
int height = 512;

GLFWwindow *window;


void glfw_window_size_callback(GLFWwindow *window, int w, int h) {
        width = w;
        height = h;

        // update perspective matrices here
}

void glfw_error_callback(int error, const char *description) {
        log_err("GLFW ERROR: code ", error, " msg: ", description);
}

void update_fps_counter(GLFWwindow* window) {
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

bool init() {
        // get that long file a'rollin
        restart_log();

        log("starting GLFW with version: ", glfwGetVersionString());

        // tell GLFW how to log errors
        glfwSetErrorCallback(glfw_error_callback);

        // start GL context using GLFW helper
        if (!glfwInit()) {
                log_err("ERROR: could not start GLFW");
                return false;
        }

        // put any window hints here
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        // this one sets anti-aliasing passes to 4
        // 16 is a good level for screenshots
        glfwWindowHint( GLFW_SAMPLES, 4 );

        window = glfwCreateWindow(width, height, "computa", NULL, NULL);

        if (NULL == window) {
                log_err("ERROR: could not create window with GLFW");
                glfwTerminate();
                return false;
        }

        glfwMakeContextCurrent(window);
        
        // tell GLFW what to do when the window is resized
        glfwSetFramebufferSizeCallback(window, glfw_window_size_callback);

        // start GLEW, experimental encourages it to use newer (4.* +) versions of OpenGL
        glewExperimental = GL_TRUE;
        glewInit();

        // only draw onto a pixel if the shape is closer to the viewer
        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LESS);    // depth-testing interpretes a smaller value as "closer"

        // some key data for the log
        const GLubyte *renderer = glGetString(GL_RENDERER);
        const GLubyte *version = glGetString(GL_VERSION);
        log("renderer: ", renderer, "\nOpenGL version supported: ", version, "\n-------------");

        return true;
}

#endif