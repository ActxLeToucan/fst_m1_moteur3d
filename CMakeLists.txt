cmake_minimum_required(VERSION 3.17)
project(fst_m1_moteur3d)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.0
)

FetchContent_MakeAvailable(glm)

file(GLOB SOURCES
        "*.h"
        "*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} glm::glm)
