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

target_link_libraries(ImageSplicing PRIVATE
    Qt5::Widgets
    ${OpenCV_LIBS}
)
target_include_directories(ImageSplicing PRIVATE ${OpenCV_INCLUDE_DIRS})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
