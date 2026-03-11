# Aurora Git Client

<div align="center">

![Aurora](resources/icons/app.png)

**A modern, simple, and powerful Git client built with Qt**

[![Build Status](https://github.com/AbstractMelon/Aurora/workflows/Build%20and%20Release/badge.svg)](https://github.com/AbstractMelon/Aurora/actions)
[![License](https://img.shields.io/github/license/AbstractMelon/Aurora)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)]()

</div>

## Features

- **Modern UI**: Clean, intuitive interface with a dark theme
- **Fast & Lightweight**: Built with C++ and Qt for optimal performance
- **Full Git Support**: All essential Git operations at your fingertips
- **GitHub Integration**: Seamless GitHub API integration for PRs, issues, and more
- **Cross-Platform**: Works on Windows, Linux, and macOS
- **Visual History**: Beautiful commit history visualization
- **Branch Management**: Easy branch creation, switching, and merging
- **Commit Management**: Stage, unstage, and commit with ease
- **Diff Viewer**: View changes before committing
- **Customizable**: Configure Git settings and preferences

## Screenshots

*Coming soon*

## Getting Started

### Prerequisites

- **Qt 6.5+**: The Qt framework
- **CMake 3.16+**: Build system
- **libgit2**: Git operations library
- **C++17 compiler**: GCC, Clang, or MSVC

### Installation

#### From Release (Recommended)

Download the latest release for your platform from the [Releases](https://github.com/AbstractMelon/Aurora/releases) page:

- **Windows**: `Aurora-Setup.exe` or `Aurora-Windows.zip`
- **Linux**: `Aurora.deb` or `Aurora.tar.gz`
- **macOS**: `Aurora.dmg`

#### Building from Source

##### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y qt6-base-dev cmake ninja-build libgit2-dev

# Clone the repository
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora

# Build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Install (optional)
sudo cmake --install build
```

##### macOS

```bash
# Install dependencies
brew install qt6 cmake ninja libgit2

# Clone the repository
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora

# Build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Create app bundle
cd build
cpack -G DragNDrop
```

##### Windows

```powershell
# Install Qt 6 from https://www.qt.io/download

# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
.\vcpkg install libgit2:x64-windows

# Clone Aurora
cd ..
git clone https://github.com/AbstractMelon/Aurora.git
cd Aurora

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release

# Create installer
cd build
cpack -G NSIS
```

## Usage

### Opening a Repository

1. **Clone**: File → Clone Repository
2. **Open**: File → Open Repository
3. **Create**: File → Create Repository

### Making Changes

1. Make changes to your files
2. View changes in the **Changes** tab - files are organized in a tree view showing all subdirectories
3. Stage files by clicking the checkbox next to each file, or use **Stage All**
4. View diffs for individual files by selecting them
5. Click **Commit** button to open the commit dialog
6. Enter a commit message (required)
7. Optionally expand **Advanced Options** to:
   - Override author name and email
   - Set custom commit date/time
8. Click **Commit** to finalize
9. Push to remote with **Repository → Push**

### Branch Management

1. Go to the **Branches** tab to see all local and remote branches
2. Click **New Branch** to create a branch from HEAD or another branch
3. Use **Repository → Branch → Switch Branch** to checkout a different branch
4. Select a branch and click **Checkout** to switch to it
5. Delete branches with the **Delete** button (cannot delete current branch)

### GitHub Integration

1. Go to **File → Settings → GitHub**
2. Enter your Personal Access Token
3. Access GitHub features through the GitHub menu

## Configuration

Aurora stores settings in:
- **Linux**: `~/.config/AbstractMelon/Aurora.conf`
- **Windows**: `%APPDATA%\AbstractMelon\Aurora.ini`
- **macOS**: `~/Library/Preferences/com.abstractmelon.Aurora.plist`

### Git Configuration

Set your Git user information in **File → Settings → Git**:
- User Name
- User Email

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Development

### Building for Development

```bash
# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Run
./build/Aurora
```

### Known Issues

- Pull operation needs merge conflict resolution UI
- GitHub API features are implemented but not yet exposed in the UI
- Custom author/date in commit dialog is UI-only (backend implementation pending)
- No syntax highlighting in diff view yet (planned)

### Roadmap

- [ ] Enhanced diff viewer with syntax highlighting
- [ ] Merge conflict resolution UI
- [ ] Stash management
- [ ] Submodule support
- [ ] Tag management
- [ ] Cherry-pick support
- [ ] Interactive rebase
- [ ] Git LFS support
- [ ] Multiple repository tabs
- [ ] Blame view
- [ ] Search in history

---

<div align="center">
Made with ❤️ by AbstractMelon
</div>
