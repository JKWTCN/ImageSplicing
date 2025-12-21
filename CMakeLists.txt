cmake_minimum_required(VERSION 3.10)

project(ImageSplicing LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置 OpenCV 路径
find_package(OpenCV REQUIRED)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

# 打印 OpenCV 信息用于调试
message(STATUS "OpenCV library status:")
message(STATUS "version: ${OpenCV_VERSION}")
message(STATUS "libraries: ${OpenCV_LIBS}")
message(STATUS "include path: ${OpenCV_INCLUDE_DIRS}")

add_executable(ImageSplicing
    WIN32
    resource.rc
    main.cpp
    tools.hpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    settingwindow.cpp
    settingwindow.h
    settingwindow.ui
    config.hpp
    imagepro.hpp
    enumerate.h
    enumerate.cpp
    SplicingLine.h
    SplicingLine.cpp
    MovablePixmapItem.h
    MovablePixmapItem.cpp
    aboutdialog.h
    aboutdialog.cpp
    aboutdialog.ui
    ${TS_FILES}
)

# --- Generate version resource from template ---
set(MAJOR 1)
set(MINOR 1)
set(PATCH 0)

configure_file(${CMAKE_SOURCE_DIR}/version.rc.in ${CMAKE_BINARY_DIR}/version.rc @ONLY)

target_sources(ImageSplicing PRIVATE ${CMAKE_BINARY_DIR}/version.rc)

# Set target version properties so the executable has version info
set_target_properties(ImageSplicing PROPERTIES
    VERSION "${MAJOR}.${MINOR}.${PATCH}"
    SOVERSION "${MAJOR}"
)

# 设置输出目录
set_target_properties(ImageSplicing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(ImageSplicing PRIVATE
    Qt5::Widgets
    ${OpenCV_LIBS}
)
target_include_directories(ImageSplicing PRIVATE ${OpenCV_INCLUDE_DIRS})

# 复制可执行文件到bin目录（确保在不同构建配置下都能工作）
add_custom_command(TARGET ImageSplicing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ImageSplicing> ${CMAKE_SOURCE_DIR}/bin/
    COMMENT "Copying executable to bin directory"
)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
