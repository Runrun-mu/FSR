# FSR (FidelityFX Super Resolution) Replication Project

This project aims to replicate AMD's FSR (FidelityFX Super Resolution) algorithm. We use OpenGL for graphics rendering and WSL (Windows Subsystem for Linux) for environment setup and running.

## Project Introduction

This project is a replication of AMD's FSR algorithm. FSR is an efficient image super-resolution algorithm suitable for gaming and other real-time image rendering applications. We use OpenGL for real-time image rendering to showcase the effects of the FSR algorithm.

## Running Steps:

### 1. Set Up OpenGL Environment

First, install the necessary libraries in WSL:

```bash
sudo apt-get install build-essential libgl1-mesa-dev
sudo apt-get install freeglut3-dev
sudo apt-get install libglew-dev libsdl2-dev libsdl2-image-dev libglm-dev libfreetype6-dev
sudo apt-get install libglfw3-dev
```

Test if OpenGL is configured correctly:

```bash
g++ version.cpp -lGL -lGLU -lglut -o version
```

Depending on your OpenGL version, download the corresponding library from [https://glad.dav1d.de/](https://glad.dav1d.de/) and unzip it to the directory at the same level as your project.

Then, move the library files to the system path:

```bash
cd glad/include
sudo mv glad/ /usr/local/include
sudo mv KHR/ /usr/local/include

//update
sudo rm -rf /usr/local/include/glad
sudo rm -rf /usr/local/include/KHR
sudo mv glad/ /usr/local/include
sudo mv KHR/ /usr/local/include
```

Configure CMake:

```makefile
cmake_minimum_required(VERSION 3.11)
project(fsr)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
include_directories( ${GLEW_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/external)

add_executable(fsr glad.c main.cpp) 

target_link_libraries(fsr ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw dl)
```

Upgrade to 3.3:

```bash
export MESA_GL_VERSION_OVERRIDE=3.3
unset LIBGL_ALWAYS_INDIRECT 

glxinfo | grep "OpenGL version" 
```

### 2. Set Up VcXsrv

Download and install [VcXsrv](https://sourceforge.net/projects/vcxsrv/) in Windows.

Launch settings:
- Choose "Multiple windows" mode
- No need to specify display number
- In "Extra settings", select "Disable access control"

### 3. Set Up WSL

Edit the /etc/resolv.conf file, note the IP address corresponding to the nameserver:

```bash
sudo vim /etc/resolv.conf
```

Add this IP to .bashrc:

```bash
sudo vim ~/.bashrc

//Add the following two lines at the end of the file
export DISPLAY=Your IP:0
export PULSE_SERVER=TCP:Your IP

source ~/.bashrc
```

Install xfce4:

```bash
sudo apt-get update
sudo apt install -y xfce4
```

Launch xfce4:

```makefile
sudo startxfce4
```

If you encounter any problems, please refer to [WSL2 startxfce4 error](https://zhuanlan.zhihu.com/p/587868390) for troubleshooting.

