cmake_minimum_required(VERSION 3.27)
project(Stacy_NN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(get_cpm.cmake)

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

CPMAddPackage(
        NAME Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4-rc1
        VERSION 3.4.0
        DOWNLOAD_ONLY YES
)


# File copies remain the same
file(COPY ../Stacy-NN/Data/set1-images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/set1-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/Testing-Data-Images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/TestingData-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/WEB DESTINATION ${CMAKE_BINARY_DIR})

add_executable(Stacy_NN Neural_Network/main.cpp Neural_Network/main.h Neural_Network/test_network.cpp)
add_executable(SVM-WEB SVM/SVM.cpp SVM/SVM.h SVM/Test.cpp SVM/MNIST.cpp SVM/MNIST.h)
add_executable( SVM-Terminal SVM/SVM.cpp SVM/SVM.h SVM/Main_SVM.cpp SVM/MNIST.cpp SVM/MNIST.h)


target_include_directories(Stacy_NN PUBLIC Neural_Network ${CMAKE_BINARY_DIR}/_deps/eigen-src)

target_compile_definitions(SVM-WEB PRIVATE _WIN32_WINNT=0x0601)

target_link_libraries(SVM-WEB PRIVATE Crow::Crow asio::asio ws2_32 mswsock)
target_link_libraries(Stacy_NN PUBLIC asio::asio ws2_32 mswsock)