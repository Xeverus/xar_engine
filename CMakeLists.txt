cmake_minimum_required(VERSION 3.25)
project(xar_engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(applications)
add_subdirectory(engine)
