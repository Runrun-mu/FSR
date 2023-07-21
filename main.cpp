#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

GLuint textureID;
GLuint framebuffer;
GLuint shaderProgram;

int scale = 2;
std::string easu = "easu.glsl";
std::string rcas = "rcas.glsl";
std::string vertex = "screen.glsl";

GLuint loadTexture(const char *imagepath) {
    int width, height, channels;
    unsigned char *image = stbi_load(imagepath, &width, &height, &channels, 0);

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);

    return textureID;
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

void setUniform(GLuint program, const std::string &name, float v0, float v1, float v2, float v3) {
    GLuint location = glGetUniformLocation(program, name.c_str());
    glUniform4f(location, v0, v1, v2, v3);
}



int main(){
    /*init*/
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();

    /*load image*/
    int width, height, channels;
    unsigned char* data = stbi_load("path_to_your_image.png", &width, &height, &channels, STBI_rgb_alpha);
    int outputWidth = width * scale;
    int outputHeight = height * scale;

    /*create texture*/
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //放大时线性插值
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);


    GLuint program1 = CreateProgram(vertex, easu);
    GLuint program2 = CreateProgram(vertex, rcas);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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

    GLuint fboTexture;
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glUseProgram(program1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(program2);

}