# Directionality Indicator

Visualization of directional information on anatomical surfaces. The project is work-in-progress. More information and functionality will follow soon.

## Build-Dependencies

To compile, you will need the following libraries and software installed on your system. The packages are available via your Linux distribution's
repository, MacPorts, and MSYS2.

* Qt5
* Qt5 Webkit for embedded help
* Qt5OpenGL for 3D graphics
* CMake >= 3.0
* CLang or GCC, supporting basic C++11 functionality.
* GLEW >= 1.7.0


## Runtime-Dependencies

* OpenGL 3.3 Core

## Building

The software uses CMake to create Makefiles. So, once you downloaded the source, you need to follow these steps:

```shell
cd /path/to/DirectionalityIndicator
# As we use out-of-source builds: Change to the build sub-directory
cd build
# Allow CMake to build and update the Makefiles. Use this command everytime you do a git pull.
cmake ../src
# Build using make
make
# Run the software
./DirectionalityIndicator
```
