cmake_minimum_required(VERSION 3.5)

project(cpp_concurrency)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")


add_subdirectory(ch02_managing_threads)

add_subdirectory(ch03_sharing_data_between_threads)

add_subdirectory(ch04_synchronizing_concurrent_operations)