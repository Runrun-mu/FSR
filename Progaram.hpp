#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "Utils.hpp"
#include "Shader.hpp"
using namespace std;

class progaram{
    public:
        static GLuint activeId;
        static void active(GLuint active){
            glUseProgram(active);
            activeId = active;
        }
    public:
        GLuint progaramID;
        unordered_map<string, GLuint> uniformMap;
    progaram(){}
    ~progaram(){
        glDeleteProgram(progaramID);
    }
    void    setUniform(const string& name);
    void    setUniform1i(const string& name, const GLint value);
    void    setUniform4fv(const string& name, const GLfloat* value);
    void    createProgaram(shared_ptr<shader> shader);
};