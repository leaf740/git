cmake_minimum_required(VERSION 3.20)
project(stm32_cpp17_framework LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(framework_core STATIC
    BSP/src/log.cpp
    Module/src/i2c_register_device.cpp
    Application/src/main.cpp)
target_include_directories(framework_core PUBLIC
    BSP/include
    Module/include
    Application/include)
