#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

GLuint createShader(GLenum shaderType, const std::string &source) {
    GLuint shader = glCreateShader(shaderType);
    const char *source_c_str = source.c_str();
    glShaderSource(shader, 1, &source_c_str, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint createShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource) {
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void drawQuad(GLuint vao) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

GLuint createFullScreenQuad() {
    GLfloat vertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}

int main(){
    /*init*/
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    /*load image*/
    int scale = 2;
    int width, height, channels;
    unsigned char* data = stbi_load("1.png", &width, &height, &channels, STBI_rgb_alpha);
    int outputWidth = width * scale;
    int outputHeight = height * scale;

    /*create texture*/
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear interpolation when magnifying
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    /*create shader program*/
    std::string EASU = readGLSLFile("easu.glsl");
    std::string RSCA = readGLSLFile("rcas.glsl");
    std::string nomal = readGLSLFile("screen.glsl");
    GLuint program1 = createShaderProgram(nomal, EASU);
    GLuint program2 = createShaderProgram(nomal, RSCA);

    /*create framebuffer*/
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint fboTexture;
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*render*/
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program1);
    glm::vec4 Con0 = {width * 1.0 / outputWidth, height * 1.0 / outputHeight, 0.5 * width / outputWidth - 0.5, 0.5 * height / outputHeight - 0.5};
    glm::vec4 Con1 = {1.0 / width, 1.0 / height, 1.0 * 1.0 / width, -1.0 * 1.0 / height};
    glm::vec4 Con2 = {-1.0 * 1.0 / width, 2.0 * 1.0 / height, 1.0 * 1.0 / width, 2.0 * 1.0 / height};
    glm::vec4 Con3 = {0.0 * 1.0 / width, 4.0 * 1.0 / height, 0, 0};
    glUseProgram(program1);
    glUniform1i(glGetUniformLocation(program1, "u_InputTexture"), 0); // Texture unit 0
    glUniform1i(glGetUniformLocation(program1, "u_DisplayWidth"), width);
    glUniform1i(glGetUniformLocation(program1, "u_DisplayHeight"), height);
    glUniform4f(glGetUniformLocation(program1, "u_Con0"), Con0[0], Con0[1], Con0[2], Con0[3]);
    glUniform4f(glGetUniformLocation(program1, "u_Con1"), Con1[0], Con1[1], Con1[2], Con1[3]);
    glUniform4f(glGetUniformLocation(program1, "u_Con2"), Con2[0], Con2[1], Con2[2], Con2[3]);
    glUniform4f(glGetUniformLocation(program1, "u_Con3"), Con3[0], Con3[1], Con3[2], Con3[3]);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLuint vao = createFullScreenQuad();
    drawQuad(vao);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    unsigned char* pixels = new unsigned char[width * height * 3]; // Assuming 3 bytes for RGB
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_write_png("output.png", width, height, 3, pixels, width * 3);
    delete[] pixels;
    return 0;
}