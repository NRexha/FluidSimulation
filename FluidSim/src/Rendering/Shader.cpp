#include "Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::string Shader::ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Could not open shader: " + path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::CompileShader(GLenum type, const std::string& source, const std::string& path) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, &log[0]);
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed (" + path + "):\n" + log);
    }

    return shader;
}

void Shader::LinkProgram(GLuint shaderA, GLuint shaderB) {
    programID = glCreateProgram();
    glAttachShader(programID, shaderA);
    if (shaderB) glAttachShader(programID, shaderB);

    glLinkProgram(programID);

    GLint success = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if (!success) {
        GLint length = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetProgramInfoLog(programID, static_cast<GLsizei>(log.size()), nullptr, &log[0]);
        glDeleteProgram(programID);
        programID = 0;
        throw std::runtime_error("Shader linking failed:\n" + log);
    }

    glDetachShader(programID, shaderA);
    if (shaderB) glDetachShader(programID, shaderB);
    glDeleteShader(shaderA);
    if (shaderB) glDeleteShader(shaderB);
}

Shader::Shader(const std::string& computePath) {
    GLuint compute = CompileShader(GL_COMPUTE_SHADER, ReadFile(computePath), computePath);
    try {
        LinkProgram(compute);
    }
    catch (...) {
        glDeleteShader(compute);
        throw;
    }
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertex = CompileShader(GL_VERTEX_SHADER, ReadFile(vertexPath), vertexPath);
    GLuint fragment = 0;

    try {
        fragment = CompileShader(GL_FRAGMENT_SHADER, ReadFile(fragmentPath), fragmentPath);
        LinkProgram(vertex, fragment);
    }
    catch (...) {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        throw;
    }
}

Shader::~Shader() {
    if (programID) glDeleteProgram(programID);
}

void Shader::Use() const {
    glUseProgram(programID);
}

void Shader::SetInt(const char* name, int value) const
{
    GLint location = glGetUniformLocation(programID, name);

    if (location != -1)
        glUniform1i(location, value);
}


void Shader::SetFloat(const char* name, float value) const {
    GLint location = glGetUniformLocation(programID, name);
    if (location != -1) glUniform1f(location, value);
}

void Shader::SetVec2(const char* name, float x, float y) const {
    GLint location = glGetUniformLocation(programID, name);
    if (location != -1) glUniform2f(location, x, y);
}

void Shader::SetVec3(const char* name, float x, float y, float z) const
{
    GLint location = glGetUniformLocation(programID, name);

    if (location != -1)
        glUniform3f(location, x, y, z);
}
