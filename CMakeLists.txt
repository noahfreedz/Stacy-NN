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

<<<<<<< HEAD
# Copy all required SFML DLLs to the build directory
file(GLOB SFML_DLLS "${SFML_DLL_PATH}/*.dll")
file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add Eigen - note the DOWNLOAD_ONLY YES flag!
=======
file(COPY ../Stacy-NN/Data/set1-images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/set1-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/Testing-Data-Images.idx3-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../Stacy-NN/Data/TestingData-labels.idx1-ubyte DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

>>>>>>> ea141bf4538815d2597b6246e1f239c6b8faac90
CPMAddPackage(
        NAME Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4-rc1
        VERSION 3.4.0
        DOWNLOAD_ONLY YES  # This is crucial - Eigen is header-only!
)

# Add the executable target, including both source files
add_executable(Stacy_NN
        "Neural Network/main.cpp"         # Test file
        "test_network.cpp"
)

<<<<<<< HEAD
# Add include directories for Neural Network and Eigen
target_include_directories(Stacy_NN
        PUBLIC
        "Neural Network"
        ${CMAKE_BINARY_DIR}/_deps/eigen-src
)

# Link SFML libraries
target_link_libraries(Stacy_NN
        PUBLIC
        sfml-graphics
        sfml-window
        sfml-system
)
=======
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
>>>>>>> ea141bf4538815d2597b6246e1f239c6b8faac90
