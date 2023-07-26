#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "external/stb_image.h"
#include "external/std_image_write.h"

using namespace std;

GLFWwindow*     initWindows(const string& name, int major = 4, int minor = 3, int width = 800, int height = 600);
unsigned char*  loadData(const string& path, int& width, int& height, int& channels);
void            saveData(const string& path, int& width, int& height, const vector<float>& data);
string          readGLSLFile(const string& filename);