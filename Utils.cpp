
#include "Utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/std_image_write.h"
using namespace std;

GLFWwindow* initWindows(const string& name, int major, int minor, int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    return window;
}

unsigned char* loadData(const string& path, int& width, int& height, int& channels) {
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (data == NULL) {
        std::cerr << "Failed to load image" <<  stbi_failure_reason() << std::endl;
        return NULL;
    }
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid image dimensions" << std::endl;
        return NULL;
    }
    return data;
}

void saveData(const string& path, int& width, int& height, const vector<float>& data){
    stbi_write_png(path.c_str(), width, height, 4, data.data(), 4 * width);
}

string readGLSLFile(const string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
