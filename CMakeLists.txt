cmake_minimum_required(VERSION 3.27)
project(Stacy_NN)

set(CMAKE_CXX_STANDARD 17)

# Include CPM for package management
include(get_cpm.cmake)

# Add SFML
CPMAddPackage("gh:SFML/SFML#2.6.1")

# Define the path to the SFML library directory
set(SFML_DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/_deps/sfml-build/lib")

# Copy all required SFML DLLs to the build directory
file(GLOB SFML_DLLS "${SFML_DLL_PATH}/*.dll")
file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add Eigen - note the DOWNLOAD_ONLY YES flag!
CPMAddPackage(
        NAME Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4-rc1
        VERSION 3.4.0
        DOWNLOAD_ONLY YES  # This is crucial - Eigen is header-only!
)

add_executable(Stacy_NN
        "Neural Network/main.cpp"
        "Neural Network/main.h")

target_include_directories(Stacy_NN
        PUBLIC
        "Neural Network"
        ${CMAKE_BINARY_DIR}/_deps/eigen-src
)

# Remove Eigen from target_link_libraries since it's header-only
target_link_libraries(Stacy_NN
        PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
)