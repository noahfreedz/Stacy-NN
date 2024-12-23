cmake_minimum_required(VERSION 3.27)
project(Stacy_NN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(get_cpm.cmake)

set(ASIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/asio/include)

CPMAddPackage(
        NAME asio
        URL https://github.com/chriskohlhoff/asio/archive/asio-1-24-0.tar.gz
        VERSION 1.24.0
        DOWNLOAD_ONLY YES
)

if (asio_ADDED)
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
    add_library(asio::asio ALIAS asio)
endif()

CPMAddPackage(
        NAME Crow
        GITHUB_REPOSITORY CrowCpp/Crow
        VERSION 1.2.0
        OPTIONS
        "CROW_BUILD_EXAMPLES OFF"
        "CROW_BUILD_TESTS OFF"
        "CROW_INSTALL OFF"
)

file(COPY ../Stacy-NN/Data/set1-images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/set1-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/Testing-Data-Images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/TestingData-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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
        SVM/Test.cpp
)

target_include_directories(Stacy_NN PUBLIC
        "Neural Network"
        ${CMAKE_BINARY_DIR}/_deps/eigen-src
        ${ASIO_INCLUDE_DIR}
)

target_include_directories(SVMTEST PUBLIC
        ${ASIO_INCLUDE_DIR}
)

target_compile_definitions(SVMTEST PRIVATE
        _WIN32_WINNT=0x0601
)

target_link_libraries(SVMTEST PRIVATE
        Crow::Crow
)

target_link_libraries(Stacy_NN PUBLIC
)