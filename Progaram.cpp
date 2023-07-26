#include "Progaram.hpp"

GLuint progaram::activeId = 0;

void progaram::createProgaram(shared_ptr<shader> shader){
    progaramID = glCreateProgram();
    for(auto& shader : shader->shaderMap)
        glAttachShader(progaramID, shader.second);
    glLinkProgram(progaramID);

    GLint status;
    glGetProgramiv(progaramID, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[512];
        glGetProgramInfoLog(progaramID, 512, NULL, buffer);
        std::cerr << "Program linking failed: " << buffer << std::endl;
    }
}

void progaram::setUniform(const string& name){
    uniformMap[name] = glGetUniformLocation(progaramID, name.c_str());
}

void progaram::setUniform1i(const string& name, const GLint value){
    if(uniformMap.count(name)){
        glUniform1i(uniformMap[name], value);
    }
    else{
        cerr << "ERROR::UNIFORM::NAME_EXISTED" << endl;
    }
}

void progaram::setUniform4fv(const string& name, const GLfloat* value){
    if(uniformMap.count(name)){
        glUniform4fv(uniformMap[name], 1, value);
    }
    else{
        cerr << "ERROR::UNIFORM::NAME_EXISTED" << endl;
    }
}