#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <glad/glad.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Utils.hpp"
#include "Shader.hpp"
#include "Progaram.hpp"
#include "ResourceManager.hpp"


int main(){
    string name = "FSR";
    GLFWwindow* window = initWindows(name);

    int scale = 2;
    int width, height, channels;
    unsigned char* data = loadData("../img/1.jpg", width, height, channels);
    int outputWidth = width * scale;
    int outputHeight = height * scale;

    if (!glfwGetCurrentContext()) {
        std::cerr << "No current OpenGL context" << std::endl;
        return -1;
    }
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    shared_ptr<texture> input_texture = make_shared<texture>();
    input_texture->setTextureUnit(0);
    input_texture->setImageUnit(2);
    input_texture->setData(data);
    input_texture->setWidth(width);
    input_texture->setHeight(height);
    input_texture->buildTexture();
    texture::setActive(input_texture->textureUnit);
    cerr << input_texture->textureID << endl;

    shared_ptr<texture> easu_texture = make_shared<texture>();
    easu_texture->setTextureUnit(1);
    easu_texture->setImageUnit(0);
    easu_texture->setWidth(outputWidth);
    easu_texture->setTypeWrapS(GL_CLAMP_TO_EDGE);
    easu_texture->setTypeWrapT(GL_CLAMP_TO_EDGE);
    easu_texture->setHeight(outputHeight);
    easu_texture->setDataType(GL_FLOAT);
    easu_texture->setReadType(GL_WRITE_ONLY);
    easu_texture->buildTexture();
    texture::setActive(easu_texture->textureUnit);
    cerr << easu_texture->textureID << endl;

    shared_ptr<texture> output_texture = make_shared<texture>();
    output_texture->setTextureUnit(2);
    output_texture->setImageUnit(1);
    output_texture->setWidth(outputWidth);
    output_texture->setHeight(outputHeight);
    output_texture->setTypeWrapS(GL_CLAMP_TO_EDGE);
    output_texture->setTypeWrapT(GL_CLAMP_TO_EDGE);
    output_texture->setDataType(GL_FLOAT);
    output_texture->setReadType(GL_WRITE_ONLY);
    output_texture->buildTexture();
    texture::setActive(output_texture->textureUnit);
    cerr << output_texture->textureID << endl;

    shared_ptr<shader> easu_shader = make_shared<shader>();
    easu_shader->setShaderType(GL_COMPUTE_SHADER, readGLSLFile("../shader/easu.glsl"));
    easu_shader->createShader();

    shared_ptr<shader> rcas_shader = make_shared<shader>();
    rcas_shader->setShaderType(GL_COMPUTE_SHADER, readGLSLFile("../shader/rcas.glsl"));
    rcas_shader->createShader();

    shared_ptr<progaram> easu_progaram = make_shared<progaram>();
    easu_progaram->createProgaram(easu_shader);

    shared_ptr<progaram> rcas_progaram = make_shared<progaram>();
    rcas_progaram->createProgaram(rcas_shader);

    progaram::active(easu_progaram->progaramID);
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    easu_progaram->setUniform("u_InputTexture");
    easu_progaram->setUniform("u_OutputEASUTexture");
    easu_progaram->setUniform("u_DisplayWidth");
    easu_progaram->setUniform("u_DisplayHeight");
    easu_progaram->setUniform("u_Con0");
    easu_progaram->setUniform("u_Con1");
    easu_progaram->setUniform("u_Con2");
    easu_progaram->setUniform("u_Con3");
    glm::vec4 Con0 = {width * 1.0f / outputWidth, height * 1.0f / outputHeight, 0.5f * width / outputWidth - 0.5f, 0.5f * height / outputHeight - 0.5f};
    glm::vec4 Con1 = {1.0f / width, 1.0f / height, 1.0f * 1.0f / width, -1.0f * 1.0f / height};
    glm::vec4 Con2 = {-1.0f * 1.0f / width, 2.0f * 1.0f / height, 1.0f * 1.0f / width, 2.0f * 1.0f / height};
    glm::vec4 Con3 = {0.0f * 1.0f / width, 4.0 * 1.0 / height, 0, 0};

    texture::setActive(input_texture->textureUnit);
    glBindTexture(GL_TEXTURE_2D, input_texture->textureID);
    easu_progaram->setUniform1i("u_InputTexture", input_texture->textureUnit);
    cerr << input_texture->textureUnit << endl;
    easu_progaram->setUniform1i("u_OutputEASUTexture", easu_texture->imageUnit);
    cerr << "easu" << easu_texture->imageUnit << endl;
    easu_progaram->setUniform1i("u_DisplayWidth", outputWidth);
    easu_progaram->setUniform1i("u_DisplayHeight", outputHeight);
    easu_progaram->setUniform4fv("u_Con0", glm::value_ptr(Con0));
    easu_progaram->setUniform4fv("u_Con1", glm::value_ptr(Con1));
    easu_progaram->setUniform4fv("u_Con2", glm::value_ptr(Con2));
    easu_progaram->setUniform4fv("u_Con3", glm::value_ptr(Con3));

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDispatchCompute(outputWidth/32, outputHeight/32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    progaram::active(rcas_progaram->progaramID);
    rcas_progaram->setUniform("u_RASUTexture");
    rcas_progaram->setUniform("u_OutputRCASImage");
    rcas_progaram->setUniform("u_Con0");
    rcas_progaram->setUniform1i("u_RASUTexture", easu_texture->textureUnit);
    cerr << easu_texture->textureUnit << endl;
    rcas_progaram->setUniform1i("u_OutputRCASImage", output_texture->imageUnit);
    cerr << output_texture->imageUnit << endl;
    rcas_progaram->setUniform4fv("u_Con0", glm::value_ptr(Con0));

    GLenum err1;
    while((err1 = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err1 << std::endl;
    }

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDispatchCompute(outputWidth/32, outputHeight/32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    std::vector<float> pixels(outputWidth * outputHeight * 4);
    texture::setActive(output_texture->textureUnit);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());
    saveData("../output1.png", outputWidth, outputHeight, pixels);
    return 0;
}