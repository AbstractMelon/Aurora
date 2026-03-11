# Building Aurora

This guide provides detailed instructions for building Aurora from source on different platforms.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Linux](#linux)
- [macOS](#macos)
- [Windows](#windows)
- [Build Options](#build-options)
- [Troubleshooting](#troubleshooting)

## Prerequisites

All platforms require:
- **CMake 3.16+**
- **Qt 6.5+**
- **libgit2**
- **C++17 compiler**

## Linux

### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    qt6-base-dev \
    qt6-tools-dev \
    libgit2-dev \
    pkg-config

# Clone repository
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora

# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run
./build/Aurora

# Install (optional)
sudo cmake --install build
```

### Fedora/RHEL

```bash
# Install dependencies
sudo dnf install -y \
    gcc-c++ \
    cmake \
    ninja-build \
    qt6-qtbase-devel \
    libgit2-devel \
    pkg-config

# Build steps same as above
```

### Arch Linux

```bash
# Install dependencies
sudo pacman -S --needed \
    base-devel \
    cmake \
    ninja \
    qt6-base \
    libgit2

# Build steps same as above
```

## macOS

### Using Homebrew

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake ninja qt6 libgit2

# Clone repository
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora

# Configure (Qt6 path may vary)
export Qt6_DIR=$(brew --prefix qt6)/lib/cmake/Qt6
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run
./build/Aurora.app/Contents/MacOS/Aurora

# Create DMG package
cd build
cpack -G DragNDrop
```

## Windows

### Using Visual Studio and vcpkg

```powershell
# Install Visual Studio 2019 or later with C++ support
# Download from: https://visualstudio.microsoft.com/

# Install Qt 6
# Download from: https://www.qt.io/download-qt-installer

# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install libgit2
.\vcpkg install libgit2:x64-windows

# Clone Aurora
cd C:\
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora

# Configure (adjust Qt path as needed)
cmake -B build -G "Visual Studio 16 2019" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake `
    -DCMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64

# Build
cmake --build build --config Release

# Run
.\build\Release\Aurora.exe

# Create installer (requires NSIS)
cd build
cpack -G NSIS
```

### Using MSYS2/MinGW

```bash
# Install MSYS2 from https://www.msys2.org/

# In MSYS2 MinGW 64-bit terminal:
pacman -S --needed \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-qt6 \
    mingw-w64-x86_64-libgit2

# Clone and build
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/Aurora.exe
```

## Build Options

### CMake Options

```bash
# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Release build with debug info
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Specify Qt installation
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt6

# Specify install prefix
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local

# Use specific generator
cmake -B build -G Ninja
cmake -B build -G "Unix Makefiles"
cmake -B build -G "Visual Studio 16 2019"
```

### Build Targets

```bash
# Build everything
cmake --build build

# Build specific target
cmake --build build --target Aurora

# Clean build
cmake --build build --target clean

# Install
cmake --install build

# Create package
cd build
cpack
```

## Packaging

### Linux

```bash
# DEB package
cd build
cpack -G DEB

# RPM package
cpack -G RPM

# Tarball
cpack -G TGZ
```

### macOS

```bash
# DMG image
cd build
cpack -G DragNDrop

# Bundle
cpack -G Bundle
```

### Windows

```bash
# NSIS installer (requires NSIS installed)
cd build
cpack -G NSIS

# ZIP archive
cpack -G ZIP
```

## Troubleshooting

### Qt not found

```bash
# Specify Qt path explicitly
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.0/gcc_64
```

### libgit2 not found

```bash
# Install development package
# Ubuntu/Debian:
sudo apt-get install libgit2-dev

# Fedora:
sudo dnf install libgit2-devel

# macOS:
brew install libgit2
```

### CMake version too old

```bash
# Ubuntu/Debian - add Kitware repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt-get update
sudo apt-get install cmake

# macOS:
brew install cmake

# Or download from https://cmake.org/download/
```

### Compiler errors

Ensure you have a C++17 compatible compiler:
- GCC 7+
- Clang 5+
- MSVC 2017+

### Link errors on Windows

Make sure you're using the correct architecture (x64) and matching compiler for Qt and vcpkg packages.

## Development Build

For faster iteration during development:

```bash
# Configure once with debug symbols
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Incremental builds
cmake --build build -j$(nproc)

# Run directly
./build/Aurora
```

## IDE Integration

### Qt Creator

1. Open CMakeLists.txt in Qt Creator
2. Configure project with desired kit
3. Build and run from IDE

### Visual Studio Code

1. Install CMake Tools extension
2. Open folder in VS Code
3. Select kit and configure
4. Build with CMake Tools

### CLion

1. Open project directory
2. CLion will automatically detect CMake
3. Configure and build

## Clean Build

If you encounter issues, try a clean build:

```bash
# Remove build directory
rm -rf build

# Reconfigure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Getting Help

If you encounter build issues:
1. Check this guide thoroughly
2. Search existing [GitHub Issues](https://github.com/AbstractMelon/Aurora/issues)
3. Create a new issue with:
   - Your OS and version
   - Qt version
   - CMake version
   - Complete error output
   - Steps you've tried
