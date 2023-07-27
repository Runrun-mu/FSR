#include "Shader.hpp"

using namespace std;

void shader::setShaderType(GLenum type, string source){
    if(!shaderMap.count(type)){
        shaderMap[type] = 0;
        shaderSource[type] = source;
    }
    else{
        cerr << "ERROR::SHADER::TYPE_EXISTED" << endl;
    }
}

void shader::createShader(){
    for(auto& shader : shaderMap){
        shader.second = glCreateShader(shader.first);
        cerr << "shader.first: " << shader.second << endl;
        const char *src = shaderSource[shader.first].c_str();
        glShaderSource(shader.second, 1, &src, nullptr);
        glCompileShader(shader.second);

        GLint status;
        glGetShaderiv(shader.second, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(shader.second, 512, NULL, buffer);
            std::cerr << "Shader compilation failed: " << buffer << std::endl;
        }
    }
}
