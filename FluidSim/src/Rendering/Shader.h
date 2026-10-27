#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    explicit Shader(const std::string& computePath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const;
    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;
    void SetVec2(const char* name, float x, float y) const;
    void SetVec3(const char* name, float x, float y, float z) const;

private:
    GLuint programID = 0;

    static std::string ReadFile(const std::string& path);
    static GLuint CompileShader(GLenum type, const std::string& source, const std::string& path);
    void LinkProgram(GLuint shaderA, GLuint shaderB = 0);
};
