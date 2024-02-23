#pragma once
#include "glad/gl.h"

//#define GL_GLEXT_PROTOTYPES
//#include "SDL3/SDL_opengl.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>


#include "assettypes/shader.h"

class ShaderHandler
{
public:

    static Shader Load(const char* vertexPath, const char* fragmentPath) { return get()._load(vertexPath, fragmentPath); };

    static void EnableShader(Shader shader) { get()._enableShader(shader); }
    static void DisableShader() { get()._enableShader(get()._defaultShader); }
    static void Init() { get()._init(); }

    static void SetBool(Shader shader, const std::string &name, bool value) { get()._setBool(shader, name, value); }
    static void SetInt(Shader shader, const std::string &name, int value) { get()._setInt(shader, name, value); }
    static void SetFloat(Shader shader, const std::string &name, float value) { get()._setFloat(shader, name, value); }
    static void SetVec4(Shader shader, const std::string &name, float x, float y, float z, float w) { get()._setVec4(shader, name, x, y, z, w); }

private:
    void _init();
    void _enableShader(Shader shader);

    Shader _load(const char* vertexPath, const char* fragmentPath);
    Shader _loadFromString(const char* vertexString, const char* fragmentString);

    void _setBool(Shader shader, const std::string &name, bool value);
    void _setInt(Shader shader, const std::string &name, int value);
    void _setFloat(Shader shader, const std::string &name, float value);
    void _setVec4(Shader shader, const std::string &name, float x, float y, float z, float w);

private:
    void _checkCompileErrors(unsigned int shader, std::string type);
    std::vector<Shader> _shaders;

    Shader _defaultShader;
    GLint textureUnitIndex = 0;

    void checkGLError(const char* file, int line) {
    GLenum errorCode = glGetError();
        if (errorCode != GL_NO_ERROR) {
            std::cerr << "OpenGL error in " << file << " at line " << line << ": ";
            while (errorCode != GL_NO_ERROR) {
                switch (errorCode) {
                    case GL_INVALID_ENUM:
                        std::cerr << "GL_INVALID_ENUM";
                        break;
                    case GL_INVALID_VALUE:
                        std::cerr << "GL_INVALID_VALUE";
                        break;
                    case GL_INVALID_OPERATION:
                        std::cerr << "GL_INVALID_OPERATION";
                        break;
                    case GL_STACK_OVERFLOW:
                        std::cerr << "GL_STACK_OVERFLOW";
                        break;
                    case GL_STACK_UNDERFLOW:
                        std::cerr << "GL_STACK_UNDERFLOW";
                        break;
                    case GL_OUT_OF_MEMORY:
                        std::cerr << "GL_OUT_OF_MEMORY";
                        break;
                    case GL_INVALID_FRAMEBUFFER_OPERATION:
                        std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
                        break;
                    default:
                        std::cerr << "Unknown error";
                        break;
                }
                std::cerr << " (" << errorCode << ")" << std::endl;
                errorCode = glGetError();
            }
        }
    }
public:
    ShaderHandler(const ShaderHandler&) = delete;
    static ShaderHandler& get() { static ShaderHandler instance; return instance; }
private:
    ShaderHandler();
};