cmake_minimum_required(VERSION 3.13)

project(monteCarlo VERSION 1.0)

add_subdirectory(models)
add_subdirectory(GBM)

add_executable(${PROJECT_NAME}.tsk main.cpp)

target_link_libraries(${PROJECT_NAME}.tsk
    models
    gbm
)

include_directories(${PROJECT_NAME}.tsk 
models
gbm)