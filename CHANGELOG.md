# Changelog

All notable changes to Aurora will be documented in this file.

## [0.1.0] - 2025-10-11

### Added
- Initial release of Aurora Git Client
- Modern Qt6-based UI with dark theme
- Full Git operations support via libgit2
- Repository management (clone, open, create)
- Tree view for file changes showing all subdirectories
- File staging/unstaging with checkboxes
- Commit dialog with message input
- Advanced commit options:
  - Custom author name and email override
  - Custom commit date/time
- Real-time diff viewer for changed files
- Branch management:
  - Create new branches
  - Switch between branches
  - Delete branches
  - View local and remote branches
- Commit history viewer
- Push/Pull/Fetch operations
- GitHub API integration (backend ready)
- Settings management:
  - Git user configuration
  - GitHub token storage
  - External editor preference
- Cross-platform support (Windows, Linux, macOS)
- CI/CD workflows for automated builds
- Comprehensive documentation

### Features
- **Tree View**: Files organized in hierarchical tree structure with folders
- **Color-Coded Status**: Visual indicators for added, modified, deleted files
- **Interactive Staging**: Click checkboxes to stage/unstage individual files
- **Diff Preview**: View changes before committing
- **Branch Switching**: Easy branch checkout via dialog
- **Modern UI**: Clean, intuitive interface with professional styling

### Known Limitations
- Custom author/date in commit dialog needs backend implementation
- Pull operation lacks merge conflict resolution UI
- GitHub features not yet exposed in UI menus
- Diff view is functional but lacks syntax highlighting

### Technical Details
- Built with Qt 6.5+
- Uses libgit2 for Git operations
- C++17 standard
- CMake build system
- Supports DEB, RPM, NSIS, DMG packaging
