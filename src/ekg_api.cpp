#include <ekg/ekg.hpp>

uint64_t ekg_cpu_timing::last_ticks = 0;
bool ekg_cpu_timing::clock_going_on = false;

bool ekg_cpu_timing::start() {
    last_ticks = SDL_GetTicks64();
    clock_going_on = true;

    return clock_going_on;
}

bool ekg_cpu_timing::endif(uint64_t ms) {
    clock_going_on = false;
    return SDL_GetTicks64() - last_ticks > ms;
}

void ekgapi::init() {
    EKG_ACTIVE_CALLBACK_MS_DELAY = 500;
    EKG_CPU_PLATFORM = ekgapi::cpu::X86;
}

void ekgapi::scroll(SDL_Event &sdl_event, float &y) {
    if (!ekg_cpu_timing::clock_going_on) {
        return;
    }

    switch (sdl_event.type) {
        case SDL_FINGERMOTION: {
            y = sdl_event.tfinger.y - ekg_display_touch_input.last_down_y;
        }

        case SDL_MOUSEWHEEL: {
            y = sdl_event.wheel.preciseY;
        }
    }
}

void ekgapi::send_output(const char *output) {
    switch (EKG_CPU_PLATFORM) {
        case ekgapi::cpu::X86: {
            std::string str = "[EKG] " + std::string(output);
            std::cout << str.c_str() << "\n";
            break;
        }

        case ekgapi::cpu::ARM: {
            //SDL_Log("[EKG] %s", output);
            break;
        }
    }
}

bool ekgapi::input_down_right(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return sdl_event.button.button == SDL_BUTTON_RIGHT;
        }

        case SDL_FINGERDOWN: {
            x = (float) sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = (float) sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekg_display_touch_input.last_down_x = x;
            ekg_display_touch_input.last_down_y = y;
            ekg_cpu_timing::start();

            return true;
        }
    }

    return false;
}

bool ekgapi::input_down_left(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return sdl_event.button.button == SDL_BUTTON_LEFT;
        }

        case SDL_FINGERDOWN: {
            x = (float) sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = (float) sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            ekg_display_touch_input.last_down_x = sdl_event.tfinger.x;
            ekg_display_touch_input.last_down_y = sdl_event.tfinger.y;

            return true;
        }
    }

    return false;
}

bool ekgapi::input_down_middle(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return sdl_event.button.button == SDL_BUTTON_MIDDLE;
        }
    }

    return false;
}

bool ekgapi::any_input_down(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_FINGERDOWN: {
            x = (float) sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = (float) sdl_event.tfinger.y * the_ekg_core->get_screen_height();
            return true;
        }

        case SDL_MOUSEBUTTONDOWN: {
            x = (float) sdl_event.button.x;
            y = (float) sdl_event.button.y;
            return true;
        }
    }


    return false;
}

bool ekgapi::any_input_down(SDL_Event &sdl_event) {
    return sdl_event.type == SDL_FINGERDOWN || sdl_event.type == SDL_MOUSEBUTTONDOWN;
}

bool ekgapi::input_up_right(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return sdl_event.button.button == SDL_BUTTON_RIGHT;
        }

        case SDL_FINGERUP: {
            x = (float) sdl_event.tfinger.x;
            y = (float) sdl_event.tfinger.y;

            return ekg_cpu_timing::endif(EKG_ACTIVE_CALLBACK_MS_DELAY);
        }
    }

    return false;
}

bool ekgapi::input_up_left(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return sdl_event.button.button == SDL_BUTTON_LEFT;
        }

        case SDL_FINGERUP: {
            x = (float) sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = (float) sdl_event.tfinger.y * the_ekg_core->get_screen_height();

            return true;
        }
    }

    return false;
}

bool ekgapi::input_up_middle(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEBUTTONUP: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return sdl_event.button.button == SDL_BUTTON_MIDDLE;
        }
    }

    return false;
}

bool ekgapi::any_input_up(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_FINGERUP: {
            x = (float) sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = (float) sdl_event.tfinger.y * the_ekg_core->get_screen_height();
            return true;
        }

        case SDL_MOUSEBUTTONUP: {
            x = (float) sdl_event.button.x;
            y = (float) sdl_event.button.y;
            return true;
        }
    }


    return false;
}

bool ekgapi::any_input_up(SDL_Event &sdl_event) {
    return sdl_event.type == SDL_FINGERUP || sdl_event.type == SDL_MOUSEBUTTONUP;
}

bool ekgapi::motion(SDL_Event &sdl_event, float &x, float &y) {
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            x = (float) sdl_event.motion.x;
            y = (float) sdl_event.motion.y;
            return true;
        }

        case SDL_FINGERMOTION: {
            x = (float) sdl_event.tfinger.x * the_ekg_core->get_screen_width();
            y = (float) sdl_event.tfinger.y * the_ekg_core->get_screen_height();
            return true;
        }
    }

    return false;
}

bool ekgapi::set(bool &old, bool &current, bool value) {
    current = value;

    if (old != current) {
        old = current;
        the_ekg_core->dispatch_todo_event(ekgutil::action::REFRESH);
    }

    return current;
}

bool ekgapi::set_direct(bool &old, bool &current, bool value) {
    current = value;
    old = current;
    return current;
}

void ekgapi::OpenGL::init() {
    ekgutil::log("API OpenGL initialised.");
}

void ekgapi::OpenGL::compile_program(ekgapi::OpenGL::program &program, const char *vertex_src, const char *fragment_src) {
    GLuint vertex_shader, fragment_shader;
    bool flag = true;

    if (!ekgapi::OpenGL::compile_shader(vertex_shader, GL_VERTEX_SHADER, vertex_src)) {
        ekgutil::log("Could not compile vertex shader.");
        flag = false;
    }

    if (!ekgapi::OpenGL::compile_shader(fragment_shader, GL_FRAGMENT_SHADER, fragment_src)) {
        ekgutil::log("Could not compile fragment shader.");
        flag = false;
    }

    if (flag) {
        program.program = glCreateProgram();

        glAttachShader(program.program, vertex_shader);
        glAttachShader(program.program, fragment_shader);
        glLinkProgram(program.program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        GLint program_compile_status = GL_FALSE;
        glGetProgramiv(program.program, GL_LINK_STATUS, &program_compile_status);

        if (program_compile_status != GL_TRUE) {
            char log[256];
            glGetProgramInfoLog(program.program, 256, NULL, log);

            ekgutil::log("Could not link program.");
        }

        program.compiled = program_compile_status;
    }
}

bool ekgapi::OpenGL::compile_shader(GLuint &shader_id, GLuint mode, const char *src) {
    shader_id = glCreateShader(mode);

    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);

    GLint shader_compile_status = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compile_status);

    if (shader_compile_status != GL_TRUE) {
        char log[256];
        glGetShaderInfoLog(shader_id, 256, NULL, log);
        ekgutil::log(log);
        return false;
    }

    return true;
}

void ekgapi::OpenGL::program::use() {
    glUseProgram(this->program);
}

void ekgapi::OpenGL::program::set_mat4x4(const std::string &uniform_name, float *mat4x4) {
    glUniformMatrix4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, GL_FALSE, mat4x4);
}

void ekgapi::OpenGL::program::set_int(const std::string &uniform_name, int32_t val) {
    glUniform1i(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void ekgapi::OpenGL::program::set_float(const std::string &uniform_name, float val) {
    glUniform1f(glGetUniformLocation(this->program, uniform_name.c_str()), val);
}

void ekgapi::OpenGL::program::set_vec4f(const std::string &uniform_name, const float *vec4) {
    glUniform4fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, vec4);
}

void ekgapi::OpenGL::program::set_vec2f(const std::string &uniform_name, const float* vec2) {
    glUniform2fv(glGetUniformLocation(this->program, uniform_name.c_str()), 1, vec2);
}
