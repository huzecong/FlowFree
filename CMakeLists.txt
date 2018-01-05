cmake_minimum_required(VERSION 3.1)
project(FlowFree)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        gamelogic.cpp
        gamelogic.h
        gamesolver.cpp
        gamesolver.h
        main.cpp)

# Add the path to the Qt installation/files
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/usr/local/opt/qt/")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run tools automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add the app icon
if (APPLE)
    set(ICON_NAME icon.icns)
    set(ICON_FILE icon/${ICON_NAME})
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(OS_BUNDLE MACOSX_BUNDLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})

    set(META_FILES ${ICON_FILE})
elseif (WIN32)
    # who cares
    set(OS_BUNDLE WIN32)

    set(META_FILES)
endif ()

find_package(Qt5 COMPONENTS Qml Core Quick Widgets Gui REQUIRED)

qt5_add_resources(RESOURCE_FILES qml.qrc)

add_executable(FlowFree ${OS_BUNDLE} ${SOURCE_FILES} ${META_FILES} ${RESOURCE_FILES})

qt5_use_modules(FlowFree Quick Core Qml Widgets Gui)
