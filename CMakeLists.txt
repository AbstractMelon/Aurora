cmake_minimum_required(VERSION 3.16)
project(Aurora VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Find libgit2
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 REQUIRED libgit2)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${LIBGIT2_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/git/gitclient.cpp
    src/git/gitclient.h
    src/git/gitrepository.cpp
    src/git/gitrepository.h
    src/widgets/repositoryview.cpp
    src/widgets/repositoryview.h
    src/widgets/commitview.cpp
    src/widgets/commitview.h
    src/widgets/branchview.cpp
    src/widgets/branchview.h
    src/widgets/changesview.cpp
    src/widgets/changesview.h
    src/widgets/historyview.cpp
    src/widgets/historyview.h
    src/widgets/diffview.cpp
    src/widgets/diffview.h
    src/dialogs/clonedialog.cpp
    src/dialogs/clonedialog.h
    src/dialogs/commitdialog.cpp
    src/dialogs/commitdialog.h
    src/dialogs/branchdialog.cpp
    src/dialogs/branchdialog.h
    src/dialogs/settingsdialog.cpp
    src/dialogs/settingsdialog.h
    src/github/githubclient.cpp
    src/github/githubclient.h
    src/models/commitmodel.cpp
    src/models/commitmodel.h
    src/models/filemodel.cpp
    src/models/filemodel.h
    src/utils/settings.cpp
    src/utils/settings.h
    src/utils/accountmanager.cpp
    src/utils/accountmanager.h
    src/dialogs/accountdialog.cpp
    src/dialogs/accountdialog.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(Aurora ${SOURCES} ${RESOURCES})

# Link libraries
target_link_libraries(Aurora
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    ${LIBGIT2_LIBRARIES}
)

# Link directories
link_directories(${LIBGIT2_LIBRARY_DIRS})

# Installation
install(TARGETS Aurora DESTINATION bin)

# Platform-specific settings
if(WIN32)
    set_target_properties(Aurora PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(APPLE)
    set_target_properties(Aurora PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/Info.plist
    )
endif()

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "Aurora")
set(CPACK_PACKAGE_VENDOR "AbstractMelon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern Git Client")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Aurora")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "AbstractMelon")
endif()

include(CPack)
