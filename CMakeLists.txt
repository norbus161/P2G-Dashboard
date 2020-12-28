cmake_minimum_required(VERSION 3.5)

project(P2G-Dashboard LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Charts REQUIRED)

add_subdirectory(3rdparty)
add_subdirectory(src)

set (RESOURCES resources.qrc)
add_executable(P2G-Dashboard ${HEADERS} ${SOURCE} ${RESOURCES})

target_include_directories(P2G-Dashboard PRIVATE src)
target_include_directories(P2G-Dashboard PRIVATE 3rdparty/ComLib_C_Interface/include)

target_link_libraries(P2G-Dashboard PRIVATE
    p2g
    fft
    persistence1d
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Charts)

if(UNIX)
    target_include_directories(P2G-Dashboard PRIVATE 3rdparty/sigwatch/include)
    target_link_libraries(P2G-Dashboard PRIVATE sigwatch)
endif()
