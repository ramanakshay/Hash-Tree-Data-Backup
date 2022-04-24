add_library(HashTree hashtree.cpp)

target_include_directories(HashTree PUBLIC "${PROJECT_SOURCE_DIR}/hash-library")

find_package(OpenMP REQUIRED)
target_link_libraries(HashTree PRIVATE OpenMP::OpenMP_CXX)
