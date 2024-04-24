cmake_minimum_required(VERSION 3.17)
project(fst_m1_moteur3d)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

find_package(glm QUIET)
if (NOT glm_FOUND)
    message(STATUS "glm not found, using FetchContent")
    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG        1.0.0
    )
    FetchContent_MakeAvailable(glm)
else ()
    message(STATUS "glm found")
endif ()

file(GLOB SOURCES
        "*.h"
        "*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_compile_options(${PROJECT_NAME} PRIVATE -fopenmp)
    target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm PRIVATE OpenMP::OpenMP_CXX)
    message(STATUS "OpenMP found, multithreading enabled")
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
    message(STATUS "OpenMP not found, multithreading disabled")
endif ()
