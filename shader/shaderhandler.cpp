#include "shaderhandler.h"

#include "window/windowhandler.h"

void ShaderHandler::_checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


Shader ShaderHandler::_load(const char *vertexPath, const char *fragmentPath) {
// 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {

        std::stringstream vShaderStream, fShaderStream;

        // open files

        if (vertexPath) {
            vShaderFile.open(vertexPath);
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();
            vertexCode = vShaderStream.str();
        }


        fShaderFile.open(fragmentPath);


        fShaderStream << fShaderFile.rdbuf();
        fShaderFile.close();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char *vShaderCode = nullptr;
    if (vertexPath) {
        vShaderCode = vertexCode.c_str();
    }
    const char *fShaderCode = fragmentCode.c_str();

    return _loadFromString(vShaderCode, fShaderCode);

}


Shader ShaderHandler::_loadFromString(const char* vertexString, const char* fragmentString) {
    // 2. compile shaders
    unsigned int vertex, fragment;

    if (vertexString) {
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexString, nullptr);
        glCompileShader(vertex);
    checkGLError(__FILE__, __LINE__);

//        get()._checkCompileErrors(vertex, "VERTEX");
    } else {
        vertex = get()._defaultShader.vertexShaderID;
    }
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentString, nullptr);
    glCompileShader(fragment);
//    get()._checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
//    get()._checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    Shader shader{};

    shader.ID = ID;
    shader.vertexShaderID = vertex;
    shader.fragmentShaderID = fragment;
    return shader;

}

void ShaderHandler::_enableShader(Shader shader) {
    glUseProgram(shader.ID);

    checkGLError(__FILE__, __LINE__);
}

void ShaderHandler::_init() {



}

ShaderHandler::ShaderHandler() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    // Fragment shader source
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D texture0;

        void main() {
            FragColor = texture(texture0, TexCoord);
        }
    )";

    _defaultShader = _loadFromString(vertexShaderSource, fragmentShaderSource);

}

void ShaderHandler::_setBool(Shader shader, const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), (int)value);
}

void ShaderHandler::_setInt(Shader shader, const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), value);
}

void ShaderHandler::_setFloat(Shader shader, const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(shader.ID, name.c_str()), value);
}

void ShaderHandler::_setVec4(Shader shader, const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader.ID, name.c_str()), x, y, z, w);
}