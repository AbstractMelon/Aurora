# Contributing to Aurora

Thank you for your interest in contributing to Aurora! This document provides guidelines and instructions for contributing.

## Code of Conduct

Please be respectful and constructive in all interactions with the project and community.

## How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/AbstractMelon/Aurora/issues)
2. If not, create a new issue with:
   - Clear title and description
   - Steps to reproduce
   - Expected vs actual behavior
   - System information (OS, Qt version, etc.)
   - Screenshots if applicable

### Suggesting Features

1. Check existing [Issues](https://github.com/AbstractMelon/Aurora/issues) for similar suggestions
2. Create a new issue with:
   - Clear description of the feature
   - Use cases and benefits
   - Possible implementation approach

### Pull Requests

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Make your changes following our coding standards
4. Test your changes thoroughly
5. Commit with clear messages: `git commit -m "Add feature: description"`
6. Push to your fork: `git push origin feature/your-feature-name`
7. Open a Pull Request with:
   - Clear description of changes
   - Reference to related issues
   - Screenshots/demos if applicable

## Development Setup

### Prerequisites

- Qt 6.5 or later
- CMake 3.16 or later
- libgit2
- C++17 compatible compiler

### Building

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/Aurora.git
cd Aurora

# Create build directory
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Run
./build/Aurora
```

## Coding Standards

### C++ Style

- Use C++17 features
- Follow Qt naming conventions:
  - Classes: `PascalCase`
  - Methods: `camelCase`
  - Private members: `m_camelCase`
  - Constants: `UPPER_CASE`
- Use 4 spaces for indentation (no tabs)
- Maximum line length: 100 characters
- Always use braces for control structures

### Example

```cpp
class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr);
    
    void doSomething();

private slots:
    void onButtonClicked();

private:
    void helperFunction();
    
    QString m_memberVariable;
    int m_count;
};
```

### Qt Specific

- Use Qt containers (QString, QList, etc.) for Qt-related code
- Use std containers for pure C++ logic
- Prefer signals/slots over callbacks
- Use `Q_OBJECT` macro for classes with signals/slots
- Use `tr()` for all user-facing strings

### Git Commits

- Use present tense: "Add feature" not "Added feature"
- Use imperative mood: "Move cursor to..." not "Moves cursor to..."
- First line should be 50 characters or less
- Reference issues: "Fix #123: Description"

Example:
```
Add branch comparison feature

- Implement branch diff viewer
- Add UI for selecting branches
- Update tests

Fixes #123
```

## Testing

- Test on multiple platforms if possible (Windows, Linux, macOS)
- Test with different repository states
- Verify UI responsiveness
- Check for memory leaks

## Documentation

- Update README.md if adding features
- Add comments for complex logic
- Document public APIs
- Update CHANGELOG.md

## Questions?

Feel free to ask questions by:
- Opening an issue with the "question" label
- Reaching out to maintainers

Thank you for contributing to Aurora!
