cmake_minimum_required(VERSION 3.21)
project(Hash_Tree_Data_Backup)

set(CMAKE_CXX_STANDARD 14)


add_subdirectory(hash-library)
add_subdirectory(hash-tree)

add_executable(main main.cpp)

target_link_libraries(main PUBLIC SHA256)
target_include_directories(main PUBLIC "${PROJECT_SOURCE_DIR}/hash-library")

target_link_libraries(main PUBLIC HashTree)
target_include_directories(main PUBLIC "${PROJECT_SOURCE_DIR}/hash-tree")

