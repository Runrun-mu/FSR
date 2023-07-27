#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "Utils.hpp"
using namespace std;


class texture{
    public:
        static GLint activeTextureUnite;
        static void setActive(GLint unit){
            glActiveTexture(GL_TEXTURE0 + unit);
            activeTextureUnite = unit;
        }
    public:
        GLenum internalFormat;
        GLenum externalFormat;
        GLenum dataType;
        GLsizei width;
        GLsizei height;
        GLint typeWrapS;
        GLint typeWrapT;
        GLint typeMinFilter;
        GLint typeMagFilter;
        GLint textureUnit;
        GLint imageUnit;
        GLuint textureID;
        GLenum readType;
        unsigned char* data;
    texture():internalFormat(GL_RGBA), 
            externalFormat(GL_RGBA), 
            dataType(GL_UNSIGNED_BYTE), 
            width(0), 
            height(0), 
            typeWrapS(GL_REPEAT), 
            typeWrapT(GL_REPEAT), 
            typeMinFilter(GL_LINEAR), 
            typeMagFilter(GL_LINEAR), 
            textureUnit(0), 
            imageUnit(0), 
            textureID(0), 
            readType(GL_READ_ONLY),
            data(nullptr){}
    ~texture(){
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(imageUnit, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        glDeleteTextures(1, &textureID);
        if(!data) delete[] data;
    }
    void    setInternalFormat(GLenum format);
    void    setExternalFormat(GLenum format);
    void    setDataType(GLenum type);
    void    setWidth(GLsizei w);
    void    setHeight(GLsizei h);
    void    setTypeWrapS(GLint type);
    void    setTypeWrapT(GLint type);
    void    setTypeMinFilter(GLint type);
    void    setTypeMagFilter(GLint type);
    void    setTextureUnit(GLint unit);
    void    setImageUnit(GLint unit);
    void    setData(unsigned char* data);
    void    setReadType(GLenum type);
    void    buildTexture();
};