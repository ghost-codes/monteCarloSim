add_subdirectory(thread_pool)

add_library(gbm gbm.cpp)

target_link_libraries(gbm
    thread_pool
)

include_directories(gbm
thread_pool
)