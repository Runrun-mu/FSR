#include "Progaram.hpp"

GLuint progaram::activeId = 0;

void progaram::createProgaram(shared_ptr<shader> shader){
    progaramID = glCreateProgram();
    for(auto& shaderp : shader->shaderMap){
        glAttachShader(progaramID, shaderp.second);
        cerr << "shaderID " << shaderp.second << endl;
    }
    
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
    cerr << "setuniform " << name <<  " " << uniformMap[name]<< endl;
}

void progaram::setUniform1i(const string& name, const GLint value){
    if(uniformMap.count(name)){
        glUniform1i(uniformMap[name], value);
        cerr << "uniformMap[name]: " << uniformMap[name]  << " " << value << endl;
    }
    else{
        cerr << "ERROR::UNIFORM::NAME_EXISTED" << endl;
    }
}

void progaram::setUniform4fv(const string& name, const GLfloat* value){
    if(uniformMap.count(name)){
        glUniform4fv(uniformMap[name], 1, value);
        cerr << "uniformMap[name]: " << uniformMap[name]  << " " << value[0] << " " << value[1] << " " << value[2] << " " << value[3] << endl;
    }
    else{
        cerr << "ERROR::UNIFORM::NAME_EXISTED" << endl;
    }
}