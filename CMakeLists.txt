cmake_minimum_required(VERSION 3.27)
project(Stacy_NN)

set(CMAKE_CXX_STANDARD 17)

# Include CPM for package management
include(get_cpm.cmake)

# Add SFML
CPMAddPackage("gh:SFML/SFML#2.6.1")
CPMAddPackage("gh:raysan5/raylib#5.5")
CPMAddPackage("gh:raysan5/raygui#4.0")
CPMAddPackage("gh:raysan5/rpng#1.0")

# Define the path to the SFML library directory
set(SFML_DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/_deps/sfml-build/lib")

file(COPY ../Stacy-NN/Data/set1-images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/set1-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/Testing-Data-Images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/TestingData-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy all required SFML DLLs to the build directory
file(GLOB SFML_DLLS "${SFML_DLL_PATH}/*.dll")
file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB RAYLIB_DLLS "${CMAKE_BINARY_DIR}/_deps/raylib-build/raylib/*.dll")
file(COPY ${RAYLIB_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

CPMAddPackage(
        NAME Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4-rc1
        VERSION 3.4.0
        DOWNLOAD_ONLY YES
)

add_executable(Stacy_NN
        "Neural Network/main.cpp"
        "Neural Network/main.h"
        SVM/SVM.cpp
        SVM/SVM.h
        SVM/Main_SVM.cpp)

add_executable(SVMTEST
        SVM/SVM.cpp
        SVM/SVM.h
        SVM/grid.cpp
        SVM/grid.h
        SVM/Test.cpp)

target_include_directories(Stacy_NN PUBLIC "Neural Network" ${CMAKE_BINARY_DIR}/_deps/eigen-src)


target_link_libraries(Stacy_NN PUBLIC sfml-graphics sfml-window sfml-system)
target_link_libraries(SVMTEST PUBLIC sfml-graphics sfml-window sfml-system raylib)