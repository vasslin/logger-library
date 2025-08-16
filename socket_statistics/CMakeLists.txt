
cmake_minimum_required(VERSION 3.12)

project(socket_stats_library VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0079 NEW)

add_library(socket_stats_library 
    lib/socket_log_server.cpp
    lib/socket_log_server.h
    lib/statistic_collector.h
    lib/statistic_collector.cpp
)

# target_include_directories(app_lib PUBLIC lib)

target_link_libraries(socket_stats_library PRIVATE logger_library)

add_executable(socket_stats main.cpp)

target_link_libraries(socket_stats PRIVATE socket_stats_library)