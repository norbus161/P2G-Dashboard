cmake_minimum_required(VERSION 3.5)

project(P2G-Gui LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_subdirectory(3rdparty)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Charts REQUIRED)

add_executable(P2G-Gui
    src/main.cpp
    src/radar.h
    src/radar.cpp
    src/endpoint_type.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
)

target_include_directories(P2G-Gui PRIVATE 3rdparty/ComLib_C_Interface/include)

target_link_libraries(P2G-Gui PRIVATE
    p2g
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Charts)

