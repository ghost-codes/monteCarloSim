cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(monteCarlo VERSION 1.0)

add_subdirectory(models)
add_subdirectory(monteCarlo)
add_subdirectory(thread_pool)


add_executable(${PROJECT_NAME}.tsk main.cpp)

target_link_libraries(${PROJECT_NAME}.tsk
    models
    monteCarlo
    thread_pool
)

include_directories(${PROJECT_NAME}.tsk 
models
monteCarlo
thread_pool
)