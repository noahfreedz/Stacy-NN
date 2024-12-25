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
        "BUILD_JAVA OFF"
        "BUILD_TESTS OFF"
        "BUILD_PERF_TESTS OFF"
        "BUILD_EXAMPLES OFF"
        "WITH_CUDA OFF"  # Remove if you need CUDA support
)

add_executable(MACYImage Image-Classes/Image.cpp Image-Classes/Image.h)

target_link_libraries(MACYImage PRIVATE opencv_core opencv_imgproc opencv_highgui)  # Updated library names