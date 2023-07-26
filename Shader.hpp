#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "Utils.hpp"
using namespace std;

class shader{
    public:
        unordered_map<GLenum, GLuint> shaderMap;
        unordered_map<GLenum, string> shaderSource;
    shader(){}
    ~shader(){
        for(auto& shader : shaderMap)
            glDeleteShader(shader.second);
    }
    void    setShaderType(GLenum type, string source);
    void    createShader();
};