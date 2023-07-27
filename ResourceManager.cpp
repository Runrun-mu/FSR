#include "ResourceManager.hpp"
#include "Utils.hpp"
#include "external/stb_image.h"
#include "external/std_image_write.h"

using namespace std;

GLint texture::activeTextureUnite = 0;

void texture::setInternalFormat(GLenum format){
    internalFormat = format;
}

void texture::setExternalFormat(GLenum format){
    externalFormat = format;
}

void texture::setDataType(GLenum type){
    dataType = type;
}

void texture::setWidth(GLsizei w){
    width = w;
}

void texture::setHeight(GLsizei h){
    height = h;
}

void texture::setTypeWrapS(GLint type){
    typeWrapS = type;
}

void texture::setTypeWrapT(GLint type){
    typeWrapT = type;
}

void texture::setTypeMinFilter(GLint type){
    typeMinFilter = type;
}

void texture::setTypeMagFilter(GLint type){
    typeMagFilter = type;
}

void texture::setTextureUnit(GLint unit){
    textureUnit = unit;
}

void texture::setImageUnit(GLint unit){
    imageUnit = unit;
}

void texture::setData(unsigned char* d){
    data = d;
}
void texture::setReadType(GLenum type){
    readType = type;
}

void texture::buildTexture(){
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, externalFormat, dataType, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, typeWrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, typeWrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, typeMinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, typeMagFilter);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindImageTexture(imageUnit, textureID, 0, GL_FALSE, 0, readType, GL_RGBA32F);
    if(!data) stbi_image_free(data);
}