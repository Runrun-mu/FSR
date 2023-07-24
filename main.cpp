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

GLuint createShader(GLenum shaderType, const std::string &source) {
    GLuint shader = glCreateShader(shaderType);
    const char *source_c_str = source.c_str();
    //std::cerr << source_c_str << std::endl;
    glShaderSource(shader, 1, &source_c_str, NULL);
    glCompileShader(shader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource) {
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
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
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    /*load image*/
    int scale = 2;
    int width, height, channels;
    unsigned char* data = stbi_load("../1.jpg", &width, &height, &channels, STBI_rgb_alpha);
    int outputWidth = width * scale;
    int outputHeight = height * scale;

    /*create texture*/
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
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    /*
    GLubyte* pixels1 = new GLubyte[width * height * 3]; // width and height of your texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels1);
    glBindTexture(GL_TEXTURE_2D, 0);
    for(int i = 0; i < width * height * 3; i += 3)
    {
        std::cerr<< "Red: " << static_cast<int>(pixels1[i]) << " Green: " << static_cast<int>(pixels1[i+1]) << " Blue: " << static_cast<int>(pixels1[i+2]) << std::endl;
    }*/
    std::cerr << "Success build text: " << texture << std::endl;

    float vertices[] = {
       // positions         // texture coords
       1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
       1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
      -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    }; 

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    std::string v = readGLSLFile("../test1.glsl");
    std::string f = readGLSLFile("../easu.glsl");
    //std::cerr << v << std::endl;
    GLuint program1 = createShaderProgram(v, f);
    GLint success;
    glGetProgramiv(program1, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program1, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }/*
    while (!glfwWindowShouldClose(window)) // Loop until user closes the window
    {
        // process input

        glUseProgram(program1);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // Double buffering
        glfwSwapBuffers(window);

        // Checks if any events are triggered (like keyboard input or mouse movement events)
    }*/
    glUseProgram(program1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLint inputTexLoc = glGetUniformLocation(program1, "u_InputTexture");
    glUniform1i(inputTexLoc, 0);
    GLint widthLoc = glGetUniformLocation(program1, "u_DisplayWidth");
    GLint heightLoc = glGetUniformLocation(program1, "u_DisplayHeight");
    glUniform1i(widthLoc, outputWidth); // Replace displayWidth and displayHeight with your values
    glUniform1i(heightLoc, outputHeight);
    GLint con0Loc = glGetUniformLocation(program1, "u_Con0");
    GLint con1Loc = glGetUniformLocation(program1, "u_Con1");
    GLint con2Loc = glGetUniformLocation(program1, "u_Con2");
    GLint con3Loc = glGetUniformLocation(program1, "u_Con3");
    glm::vec4 Con0 = {width * 1.0 / outputWidth, height * 1.0 / outputHeight, 0.5 * width / outputWidth - 0.5, 0.5 * height / outputHeight - 0.5};
    glm::vec4 Con1 = {1.0 / width, 1.0 / height, 1.0 * 1.0 / width, -1.0 * 1.0 / height};
    glm::vec4 Con2 = {-1.0 * 1.0 / width, 2.0 * 1.0 / height, 1.0 * 1.0 / width, 2.0 * 1.0 / height};
    glm::vec4 Con3 = {0.0 * 1.0 / width, 4.0 * 1.0 / height, 0, 0};
    glUniform4fv(con0Loc, 1, glm::value_ptr(Con0)); // Replace con0 to con3 with your values
    glUniform4fv(con1Loc, 1, glm::value_ptr(Con1));
    glUniform4fv(con2Loc, 1, glm::value_ptr(Con2));
    glUniform4fv(con3Loc, 1, glm::value_ptr(Con3));
    GLuint outputTex;
    glGenTextures(1, &outputTex);
    glBindTexture(GL_TEXTURE_2D, outputTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, outputWidth, outputHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(0, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    //glBindTexture(GL_TEXTURE_2D, 0);
    //glBindImageTexture(0, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    GLint outputTexLoc = glGetUniformLocation(program1, "u_OutputEASUTexture");
    glUniform1i(outputTexLoc, 0);
    GLuint framebuffer;
    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex, 0);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0);
    glEnd();
    std::vector<unsigned char> outputDataChar(outputWidth * outputHeight * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, outputDataChar.data());
    for(int i = 0; i < outputDataChar.size(); i++) {
        outputDataChar[i] = static_cast<unsigned char>(outputDataChar[i] * 255.0f);
    }
    if(!stbi_write_png("output.png", outputWidth, outputHeight, 4, outputDataChar.data(), outputWidth * 4)) {
    std::cerr << "Failed to save PNG image\n";
    } 
 
    /*
    glUniform1i(glGetUniformLocation(program1, "u_InputTexture"), 0); // Texture unit 0
    glUniform1i(glGetUniformLocation(program1, "u_DisplayWidth"), width);
    glUniform1i(glGetUniformLocation(program1, "u_DisplayHeight"), height);
    glUniform4f(glGetUniformLocation(program1, "u_Con0"), Con0[0], Con0[1], Con0[2], Con0[3]);
    glUniform4f(glGetUniformLocation(program1, "u_Con1"), Con1[0], Con1[1], Con1[2], Con1[3]);
    glUniform4f(glGetUniformLocation(program1, "u_Con2"), Con2[0], Con2[1], Con2[2], Con2[3]);
    glUniform4f(glGetUniformLocation(program1, "u_Con3"), Con3[0], Con3[1], Con3[2], Con3[3]);
    */
    /*
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    std::vector<unsigned char> pixels(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    int width_in_bytes = width * 4;
    unsigned char *top = NULL;
    unsigned char *bottom = NULL;
    unsigned char temp = 0;
    int half_height = height / 2;

    for (int row = 0; row < half_height; row++) {
        top = pixels.data() + row * width_in_bytes;
        bottom = pixels.data() + (height - row - 1) * width_in_bytes;
        std::swap_ranges(top, top + width_in_bytes, bottom);
    }
    stbi_write_png("screenshot.png", width, height, 4, pixels.data(), width_in_bytes);
    /*create shader program*/
    /*
    std::string EASU = readGLSLFile("easu.glsl");
    std::string RSCA = readGLSLFile("rcas.glsl");
    std::string nomal = readGLSLFile("screen.glsl");
    GLuint program1 = createShaderProgram(nomal, EASU);
    GLuint program2 = createShaderProgram(nomal, RSCA);
    std::cerr << "Success build shader1: " << program1 << std::endl;
    std::cerr << "Success build shader2: " << program2 << std::endl;
    /*create framebuffer*/
    /*
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    std::cerr << "Success build fbuffer: " << fbo << std::endl;
    GLuint fboTexture;
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cerr << "Success build fbuffer texture: " << fboTexture << std::endl;
    /*render*/
    /*
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program1);
    glm::vec4 Con0 = {width * 1.0 / outputWidth, height * 1.0 / outputHeight, 0.5 * width / outputWidth - 0.5, 0.5 * height / outputHeight - 0.5};
    glm::vec4 Con1 = {1.0 / width, 1.0 / height, 1.0 * 1.0 / width, -1.0 * 1.0 / height};
    glm::vec4 Con2 = {-1.0 * 1.0 / width, 2.0 * 1.0 / height, 1.0 * 1.0 / width, 2.0 * 1.0 / height};
    glm::vec4 Con3 = {0.0 * 1.0 / width, 4.0 * 1.0 / height, 0, 0};
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
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    stbi_write_png("output.png", width, height, 3, pixels, width * 3);
    delete[] pixels;*/
    return 0;
}