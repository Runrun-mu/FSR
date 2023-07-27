#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glad/glad.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/std_image_write.h"

std::string readGLSLFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    int scale = 2;
    int width, height, channels;
    unsigned char* data = stbi_load("../1.jpg", &width, &height, &channels, STBI_rgb_alpha);
    int outputWidth = width * scale;
    int outputHeight = height * scale;
    if (data == NULL) {
        std::cerr << "Failed to load image" <<  stbi_failure_reason() << std::endl;
        return -1;
    }
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid image dimensions" << std::endl;
        return -1;
    }
    if (!glfwGetCurrentContext()) {
        std::cerr << "No current OpenGL context" << std::endl;
        return -1;
    }
    std::cerr << height << width << channels << std::endl;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear interpolation when magnifying
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Clamp to edge of texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Clamp to edge of texture
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(2, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    stbi_image_free(data);

    GLuint testtexture;
    glGenTextures(1, &testtexture);
    glBindTexture(GL_TEXTURE_2D, testtexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, outputWidth, outputHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge of texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Clamp to edge of texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(1, testtexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    GLuint outputTex;
    glGenTextures(1, &outputTex);
    glBindTexture(GL_TEXTURE_2D, outputTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, outputWidth, outputHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge of texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Clamp to edge of texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(0, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, outputWidth, outputHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Clamp to edge of texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Clamp to edge of texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(3, result, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    std::string f = readGLSLFile("../easu.glsl");
    const char *source_c_str = f.c_str();
    GLuint computeShader;
    computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &source_c_str, NULL);
    glCompileShader(computeShader);

    GLint status;
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(computeShader, 512, NULL, buffer);
        std::cerr << "Shader compilation failed: " << buffer << std::endl;
    }

    GLuint program;
    program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(program);

    glBindTexture(GL_TEXTURE_2D, texture);
    GLint inputTexLoc = glGetUniformLocation(program, "u_InputImage");
    glUniform1i(inputTexLoc, 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glActiveTexture(GL_TEXTURE4);
    GLint samplerLoc = glGetUniformLocation(program, "u_InputTexture");
    glUniform1i(samplerLoc, 4);
    glBindTexture(GL_TEXTURE_2D, texture);
    //glBindTexture(GL_TEXTURE_2D, 0);

    GLint widthLoc = glGetUniformLocation(program, "u_DisplayWidth");
    GLint heightLoc = glGetUniformLocation(program, "u_DisplayHeight");
    glUniform1i(widthLoc, outputWidth); // Replace displayWidth and displayHeight with your values
    glUniform1i(heightLoc, outputHeight);
    std::cerr << widthLoc << " " << heightLoc << std::endl;

    GLint con0Loc = glGetUniformLocation(program, "u_Con0");
    GLint con1Loc = glGetUniformLocation(program, "u_Con1");
    GLint con2Loc = glGetUniformLocation(program, "u_Con2");
    GLint con3Loc = glGetUniformLocation(program, "u_Con3");
    glm::vec4 Con0 = {width * 1.0f / outputWidth, height * 1.0f / outputHeight, 0.5f * width / outputWidth - 0.5f, 0.5f * height / outputHeight - 0.5f};
    glm::vec4 Con1 = {1.0f / width, 1.0f / height, 1.0f * 1.0f / width, -1.0f * 1.0f / height};
    glm::vec4 Con2 = {-1.0f * 1.0f / width, 2.0f * 1.0f / height, 1.0f * 1.0f / width, 2.0f * 1.0f / height};
    glm::vec4 Con3 = {0.0f * 1.0f / width, 4.0 * 1.0 / height, 0, 0};
    glUniform4fv(con0Loc, 1, glm::value_ptr(Con0)); // Replace con0 to con3 with your values
    glUniform4fv(con1Loc, 1, glm::value_ptr(Con1));
    glUniform4fv(con2Loc, 1, glm::value_ptr(Con2));
    glUniform4fv(con3Loc, 1, glm::value_ptr(Con3));

    GLint outputTexLoc = glGetUniformLocation(program, "u_OutputEASUTexture");
    glUniform1i(outputTexLoc, 0);
    std::cerr << outputTexLoc << std::endl;

    GLint testinputTexLoc = glGetUniformLocation(program, "u_testTexture");
    glUniform1i(testinputTexLoc, 1);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDispatchCompute(outputWidth/32, outputHeight/32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    std::string f1 = readGLSLFile("../rcas.glsl");
    const char *source_c_str1 = f1.c_str();
    //std::cerr << source_c_str1 << std::endl;
    GLuint computeShader1;
    computeShader1 = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader1, 1, &source_c_str1, NULL);
    glCompileShader(computeShader1);

    GLint status1;
    glGetShaderiv(computeShader1, GL_COMPILE_STATUS, &status1);
    if (status1 != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(computeShader1, 512, NULL, buffer);
        std::cerr << "Shader compilation failed: " << buffer << std::endl;
    }

    GLuint program1;
    program1 = glCreateProgram();
    glAttachShader(program1, computeShader1);
    glLinkProgram(program1);

    GLint success1;
    glGetProgramiv(program1, GL_LINK_STATUS, &success1);
    if (!success1) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program1, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glUseProgram(program1);

    glActiveTexture(GL_TEXTURE4);
    GLint samplerLoc1 = glGetUniformLocation(program1, "u_RASUTexture");
    glUniform1i(samplerLoc1, 4);
    glBindTexture(GL_TEXTURE_2D, outputTex);

    glBindImageTexture(3, result, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    GLint inputTexLoc1 = glGetUniformLocation(program1, "u_OutputRCASImage");
    glUniform1i(inputTexLoc1, 3);

    GLint con0Loc1 = glGetUniformLocation(program1, "u_Con0");
    glUniform4fv(con0Loc1, 1, glm::value_ptr(Con0)); // Replace con0 to con3 with your values

    GLenum err1;
    while((err1 = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err1 << std::endl;
    }

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDispatchCompute(outputWidth/32, outputHeight/32, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    std::vector<float> pixels1(outputWidth * outputHeight * 4);
    glBindTexture(GL_TEXTURE_2D, result);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels1.data());

stbi_write_png("output.png", outputWidth, outputHeight, 4, pixels1.data(), outputWidth * 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    //stbi_write_png("output1.png", outputWidth, outputHeight, 4, pixels1, outputWidth * 4 * sizeof(float));
    //delete[] pixels1;
    return 0;
}