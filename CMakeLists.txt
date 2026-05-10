cmake_minimum_required(VERSION 3.20)
project(SpectreNodes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter Development.Module)

Python_add_library(${PROJECT_NAME} MODULE main.cpp)