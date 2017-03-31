cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(fmt_motion_detection)

add_subdirectory(imreg_fmt)

find_package(OpenCV REQUIRED)

find_package(Boost 1.46 COMPONENTS
    filesystem
    system
    REQUIRED
)

include_directories(
    include/
    imreg_fmt/include
    ${OpenCV_INCLUDE_DIRS}
    /usr/include/eigen3
    ${Boost_INCLUDE_DIRS}
)

add_executable(fmt_motion_detection_image
    src/image_main.cpp
    src/motion_detection.cpp
    src/image_differencing.cpp
)

target_link_libraries(fmt_motion_detection_image
    imreg_fmt
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)

add_executable(fmt_motion_detection_video
    src/video_main.cpp
    src/motion_detection.cpp
    src/image_differencing.cpp
)

target_link_libraries(fmt_motion_detection_video
    imreg_fmt
    ${OpenCV_LIBS}
)
