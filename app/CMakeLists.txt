
cmake_minimum_required(VERSION 3.12)

project(logger_library VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0079 NEW)

add_library(app_lib 
    lib/app.cpp
    lib/app.h
    lib/ts_queue.h
)

# target_include_directories(app_lib PUBLIC lib)

target_link_libraries(app_lib PRIVATE logger_library)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE app_lib)