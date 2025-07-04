cmake_minimum_required(VERSION 3.10)  # Updated version to avoid deprecation warning

project(ImageSplicing LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

#set(TS_FILES ImageSplicing_zh_CN.ts)

# Add OpenCV find package before creating the target
list(APPEND CMAKE_PREFIX_PATH "D:\\libs\\opencv\\build\\x64\\vc16\\lib")
find_package(OpenCV REQUIRED)

include_directories(nlohmann)
add_executable(ImageSplicing
    main.cpp
    tools.cpp
    tools.h
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    settingwindow.cpp
    settingwindow.h
    settingwindow.ui
    setting.h
    setting.cpp
    enumerate.h
    enumerate.cpp
    ${TS_FILES}
    )

# Link all libraries in one command to avoid mixing styles
target_link_libraries(ImageSplicing PRIVATE
    Qt5::Widgets
    ${OpenCV_LIBS}
    )

# Include OpenCV directories
target_include_directories(ImageSplicing PRIVATE ${OpenCV_INCLUDE_DIRS})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
