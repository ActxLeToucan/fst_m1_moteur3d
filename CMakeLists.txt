cmake_minimum_required(VERSION 3.27)
project(fst_m1_moteur3d)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
        "*.h"
        "*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
