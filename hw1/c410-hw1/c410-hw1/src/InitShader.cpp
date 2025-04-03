#include "Angel.h"
#include <fstream>
#include <sstream>

namespace Angel {

// Read the shader source from a file
static char* readShaderSource(const char* shaderFile) {
    std::ifstream file(shaderFile, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << shaderFile << std::endl;
        return nullptr;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    char* shaderSource = new char[content.size() + 1];
    std::strcpy(shaderSource, content.c_str());
    return shaderSource;
}

GLuint InitShader(const char* vShaderFile, const char* fShaderFile) {
    struct Shader {
        const char* filename;
        GLenum type;
        char* source;
    } shaders[2] = {
        { vShaderFile, GL_VERTEX_SHADER, nullptr },
        { fShaderFile, GL_FRAGMENT_SHADER, nullptr }
    };

    GLuint program = glCreateProgram();

    for (int i = 0; i < 2; ++i) {
        Shader& s = shaders[i];
        s.source = readShaderSource(s.filename);
        if (s.source == nullptr) {
            std::cerr << "Failed to read " << s.filename << std::endl;
            exit(EXIT_FAILURE);
        }
        GLuint shader = glCreateShader(s.type);
        glShaderSource(shader, 1, (const GLchar**)&s.source, nullptr);
        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            std::cerr << s.filename << " failed to compile:" << std::endl;
            GLint logSize;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
            char* logMsg = new char[logSize];
            glGetShaderInfoLog(shader, logSize, nullptr, logMsg);
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
            exit(EXIT_FAILURE);
        }
        delete [] s.source;
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        std::cerr << "Shader program failed to link" << std::endl;
        GLint logSize;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog(program, logSize, nullptr, logMsg);
        std::cerr << logMsg << std::endl;
        delete [] logMsg;
        exit(EXIT_FAILURE);
    }
    
    glUseProgram(program);
    return program;
}

} // namespace Angel

