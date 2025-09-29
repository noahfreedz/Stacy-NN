cmake_minimum_required(VERSION 3.27)
project(Stacy_NN)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(get_cpm.cmake)

CPMAddPackage(
        NAME OpenCV
        GITHUB_REPOSITORY opencv/opencv
        GIT_TAG 4.10.0
        OPTIONS
        "BUILD_opencv_python OFF"
        "BUILD_opencv_python2 OFF"
        "BUILD_opencv_python3 OFF"
        "WITH_MATLAB OFF"
)

# Wait for OpenCV to be completely configured
if(TARGET opencv_core)
    add_executable(MACYImage Image-Classes/Image.cpp Image-Classes/Image.h)

    # Include directories
    target_include_directories(MACYImage PRIVATE
            "${CMAKE_BINARY_DIR}/_deps/opencv-build"
            "${CMAKE_BINARY_DIR}/_deps/opencv-build/modules/core" # Add this line
            "${CMAKE_BINARY_DIR}/_deps/opencv-src/include"
            "${CMAKE_BINARY_DIR}/_deps/opencv-src/modules/core/include"
            "${CMAKE_BINARY_DIR}/_deps/opencv-src/modules/imgproc/include"
            "${CMAKE_BINARY_DIR}/_deps/opencv-src/modules/videoio/include"
            "${CMAKE_BINARY_DIR}/_deps/opencv-src/modules/highgui/include"
    )

    # Make sure opencv_modules.hpp is generated before building your target
    add_dependencies(MACYImage opencv_core opencv_imgproc opencv_highgui opencv_videoio)

    # Link against OpenCV libraries
    target_link_libraries(MACYImage PRIVATE
            opencv_core
            opencv_imgproc
            opencv_highgui
            opencv_videoio
    )
endif()