# Directionality Indicator

Visualization of directional information on anatomical surfaces. The project is work-in-progress. More information and functionality will follow soon.

## License

The software is licensed under the terms and conditions of the LGPL v3 license.
As such, you are allowed to use the software and its source according to the GPLv3, LGPLv3, or any newer version of these licenses.

## Authors

* Sebastian Eichelbaum: [www.nemtics.com](http://www.nemtics.com)

## Installation

### Dependencies

To compile, you will need the following libraries and software installed on your system. The packages are available via your Linux distribution's
repository, MacPorts, and MSYS2.

* Qt5
* Qt5 Webkit for embedded help
* Qt5OpenGL for 3D graphics
* CMake >= 3.0
* CLang or GCC, supporting basic C++11 functionality.
* GLEW >= 1.7.0

### Runtime Dependencies

* OpenGL 3.3 Core.

### Building

The software uses CMake to create Makefiles. So, once you downloaded the source, you need to follow these steps:

```shell
$ cd /path/to/DirectionalityIndicator
# As we use out-of-source builds: Change to the build sub-directory
$ cd build
# Allow CMake to build and update the Makefiles. 
# -> Use this command everytime you do a git pull.
# -> CMake will complain about missing depenencies.
$ cmake ../src
# -> CMake does not find Qt5 on your system? Help Cmake to find it by defining the QTDIR variable:
$ QTDIR=/path/to/Qt cmake ../src
# Build using make
$ make
# Run the software
$ ./DirectionalityIndicator
```

## Usage

To start, simply start the binary.

```shell
$ DirectionalityIndicator
```

It is planned to allow specification of data files on the command line. Right now, this is not supported. More options will follow.

