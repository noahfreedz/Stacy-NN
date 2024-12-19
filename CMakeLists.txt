cmake_minimum_required(VERSION 3.27)
project(Stacy_NN)

set(CMAKE_CXX_STANDARD 14)

include_directories("Neural Network")

add_executable(Stacy_NN
        "Neural Network/main.cpp"
        "Neural Network/main.h")
